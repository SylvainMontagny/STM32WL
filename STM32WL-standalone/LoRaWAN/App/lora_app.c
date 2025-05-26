
#include "platform.h"
#include "Region.h" /* Needed for LORAWAN_DEFAULT_DATA_RATE */
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "sys_conf.h"
#include "CayenneLpp.h"
#include "sys_sensors.h"
#include "radio.h"
#include "send_raw_lora.h"
#include "stdlib.h"
#include "lcd_printf.h" /* To print logs on the LCD screen */

#include  "General_Setup.h"
uint8_t simuTemperature(void);
uint8_t simuHumidity(void);
atim_payload_t simuAtim(void);
tct_payload_t simuTct(void);
watteco_payload_t simuWatteco(void);
static void byteReception(uint8_t *PData, uint16_t Size, uint8_t Error);
#define RX_BUFF_SIZE 250

static uint8_t rxBuff[RX_BUFF_SIZE];
uint8_t isRxConfirmed;
uint32_t LoRaMode = 0;
uint8_t size_txBUFFER = 0;
uint8_t txBUFFER[100];
uint8_t isTriggered = 0;
sensor_t sensor_data = { 0,0,0,0,0,0,0,0,0,50}; // last byte is temperature setpoint in Q7.1

typedef enum TxEventType_e
{
	TX_ON_TIMER,
	TX_ON_EVENT
} TxEventType_t;

static void SendTxData(void);
static void OnTxTimerEvent(void *context);
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);
static void OnTxData(LmHandlerTxParams_t *params);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
static void OnMacProcessNotify(void);

static void OnTxTimerLedEvent(void *context);
static void OnRxTimerLedEvent(void *context);
static void OnJoinTimerLedEvent(void *context);

static void MX_BP_IT_Init(void);

static void while_loop(void);

static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

static LmHandlerCallbacks_t LmHandlerCallbacks =
{
		.GetBatteryLevel =           GetBatteryLevel,
		.GetTemperature =            GetTemperatureLevel,
		.GetUniqueId =               GetUniqueId,
		.GetDevAddr =                GetDevAddr,
		.OnMacProcess =              OnMacProcessNotify,
		.OnJoinRequest =             OnJoinRequest,
		.OnTxData =                  OnTxData,
		.OnRxData =                  OnRxData
};

static LmHandlerParams_t LmHandlerParams =
{
		.ActiveRegion =             ACTIVE_REGION,
		.DefaultClass =             LORAWAN_DEFAULT_CLASS,
		.AdrEnable =                LORAWAN_ADR_STATE,
		.TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
		.PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};

//static TxEventType_t EventType = ADMIN_TX_TYPE;
static UTIL_TIMER_Object_t TxTimer;
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
static uint8_t AppLedStateOn = RESET;
static UTIL_TIMER_Object_t TxLedTimer;
static UTIL_TIMER_Object_t RxLedTimer;
static UTIL_TIMER_Object_t JoinLedTimer;

void LoRaWAN_Init(void)
{
	MX_BP_IT_Init();

	// Starts the RX USART2 process by interrupt
	UTIL_ADV_TRACE_StartRxProcess(byteReception);

	APP_LOG_COLOR(CLEAR_SCREEN);
	APP_LOG_COLOR(RESET_COLOR);
	APP_LOG(0, 1, " \r\n");
	APP_LOG(0, 1, "########################################\r\n");
	APP_LOG(0, 1, "###### LoRaWAN Training Session ########\r\n");
	APP_LOG(0, 1, "###### Savoie Mont Blanc University ####\r\n");
	APP_LOG(0, 1, " \r\n");

	lcd_printf(LCD_DEFAULT_FONT_COLOR, "################################");
	lcd_printf(LCD_DEFAULT_FONT_COLOR, "### LoRaWAN Training Session ###");
	lcd_printf(LCD_DEFAULT_FONT_COLOR, "############# USMB #############");

	// Check board program
	// APP_LOG_COLOR(GREEN);
	// APP_LOG(0, 1, "\r\n    Your LoRaWAN device is ready\r\n\r\n");
	// while(1);

	BSP_LED_Init(LED_BLUE);
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_RED);

	BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);		// BUTTON_SW1 = PA0, IRQ number = EXTI0_IRQn
	BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_GPIO);

	/****** Raw LoRa Packet Application *************/
	if ( BSP_PB_GetState(BUTTON_SW2) == 0 || RAW_LORA_APP == true) {

		LoRaMode = 1;
		APP_LOG_COLOR(BLUE);
		APP_LOG(0, 1, "Raw LoRa Packet Application\r\n\r\n");

		APP_LOG_COLOR(WHITE);
		APP_LOG(0, 1, "Type the following command to send a Raw LoRa Packet\r\n");
		APP_LOG(0, 1, "> Command format : LORA=Frequency:Power:SF:Payload\r\n");
		APP_LOG(0, 1, "> Example :        LORA=868100000:14:7:01020304 \r\n\r\n");

		lcd_printf(LCD_BLUE, "Raw LoRa Packet Application");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "Please refer to the console");

		//lcd_printf(LCD_DEFAULT_FONT_COLOR, "LoRaWANInit Raw mode");
		lcd_print_buf();
	}

	/***** LoRaWAN Standalone Application  ***********/
	else{
		APP_LOG(0, 1, "> Activation mode         %s",(ACTIVATION_MODE == ABP) ? "ABP \r\n" : "OTAA \r\n");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "________________________________");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "Activation Mode   %s",(ACTIVATION_MODE == ABP) ? "ABP" : "OTAA");
		if(CLASS == CLASS_A){
			APP_LOG(0, 1, "> Class                   A\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Class             A");
		}
		else if(CLASS == CLASS_B){
			APP_LOG(0, 1, "> Class                   B\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Class             B");
		}
		else if(CLASS == CLASS_C){
			APP_LOG(0, 1, "> Class                   C\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Class             C");
		}
		if(SEND_BY_PUSH_BUTTON == true){
			APP_LOG(0, 1, "> Send frame              On push button event \r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Send frame        On BP");
		}
		else{
			APP_LOG(0, 1, "> Send frame every        %d ms\r\n", ADMIN_TxDutyCycleTime);
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Send frame every  %d s", ADMIN_TxDutyCycleTime/1000);
		}
		APP_LOG(0, 1, "> Spreading Factor        %d \r\n", SPREADING_FACTOR);
		APP_LOG(0, 1, "> Adaptive Data Rate      %s", (ADAPTIVE_DR == true) ? "ON \r\n" : "OFF \r\n");
		APP_LOG(0, 1, "> Uplink Frame            %s",(CONFIRMED == true) ? "Confirmed\r\n" : "Unconfirmed\r\n");
		APP_LOG(0, 1, "> App Port number         %d \r\n", (!WATTECO_TEMPO)? APP_PORT: WATTECO_TEMPO_PORT);

		lcd_printf(LCD_DEFAULT_FONT_COLOR, "Spreading Factor  %d ms", SPREADING_FACTOR);
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "ADR               %s", (ADAPTIVE_DR == true) ? "ON" : "OFF");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "Uplink Frame      %s", (CONFIRMED == true) ? "Conf" : "Unconf");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "App Port Number   %d", (!WATTECO_TEMPO)? APP_PORT: WATTECO_TEMPO_PORT);

		if(CAYENNE_LPP && SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content:        CayenneLPP, sensors\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   CLPP+se");
		}
		else if(CAYENNE_LPP && !SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content:        CayenneLPP, simulated values\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   CLPP+si");
		}
		else if(PAYLOAD_TEMPERATURE && !PAYLOAD_HUMIDITY && SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte temperature\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   Temp");
		}
		else if(PAYLOAD_TEMPERATURE && PAYLOAD_HUMIDITY && SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte temperature + humidity\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   T+H");
		}
		else if(!PAYLOAD_TEMPERATURE && PAYLOAD_HUMIDITY && SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte humidity\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   Hum");
		}
		else if(PAYLOAD_TEMPERATURE && !PAYLOAD_HUMIDITY && !SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte simulated temperature\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   si Temp");
		}
		else if(PAYLOAD_TEMPERATURE && PAYLOAD_HUMIDITY && !SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte simulated temperature + humidity\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   si T+H");
		}
		else if(!PAYLOAD_TEMPERATURE && PAYLOAD_HUMIDITY && !SENSOR_ENABLED){
			APP_LOG(0, 1, "> Payload content         1-byte simulated humidity\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   si Hum");
		}
		else if(PAYLOAD_1234 == true){
			APP_LOG(0, 1, "> Payload content         0x01 0x02 0x03 0x04\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   1234");
		}
		else if(CAYENNE_LPP == true){
			APP_LOG(0, 1, "> Payload content         CayenneLPP, sensors\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   CLPP");
		}
		else if(USMB_VALVE == true){
			APP_LOG(0, 1, "> Payload content         1 byte setpoint + 1 byte temperature\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   SP+T");
		}
		else if(ATIM_THAQ == true){
			APP_LOG(0, 1, "> Payload content         17 bytes ATIM_THAQ simulated payload\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   ATIM");
		}
		else if(WATTECO_TEMPO == true){
			APP_LOG(0, 1, "> Payload content         32 bytes WATTECO_TEMP'O fixed simulated payload\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   WATTECO");
		}
		else if(TCT_EGREEN == true){
			APP_LOG(0, 1, "> Payload content         10 bytes TCT_EGREEN simulated payload\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload content   TCT");
		}
		//APP_LOG(0, 1, "> Low Power:              %s",(LOW_POWER == true) ? "ON \r\n" : "OFF \r\n");
		APP_LOG(0, 1, "\r\n");
		lcd_printf(LCD_DEFAULT_FONT_COLOR, "");
		APP_LOG_COLOR(BLUE);

		// Display buffer on screen
		//lcd_printf(LCD_DEFAULT_FONT_COLOR, "LoRaWAN Init");
		lcd_print_buf();

		UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
		UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
		UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
		UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
		UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
		UTIL_TIMER_SetPeriod(&JoinLedTimer, 1000);

		UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
		UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);
		UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_DisplayOnLCD), UTIL_SEQ_RFU, lcd_print_buf);

		LoraInfo_Init();

		/* Initialize all callbacks */
		LmHandlerInit(&LmHandlerCallbacks);

		/* Print LoRaWAN information : DevEUI & Devaddr when ABP - DevEUI & AppEUI-JoinEUI for OTAA */
		/* Print Session Keys for ABP - Print Root Key for OTAA :LmHandlerConfigure() > LoRaMacInitialization() > SecureElementInit() > PrintKey() */
		LmHandlerConfigure(&LmHandlerParams);

		/* Let all print out terminated. Otherwise logs are affected.*/
		HAL_Delay(500);

		/* Join Red LED starts blinking */
		UTIL_TIMER_Start(&JoinLedTimer);

		/* Join procedure for OTAA */
		/* First try to Join Network. Next time the Device tries to send data (LmHandlerSend), it will check the Join.
		 * If the first Join was NOT successful, it sends another Join.
		 */
		LmHandlerJoin(ActivationType);
	}
}


// Callback for byte reception
static void byteReception(uint8_t *PData, uint16_t Size, uint8_t Error){
	static uint32_t index = 0;

	USART2->TDR = *PData;

	if ( *PData == '\r' ){
		rxBuff[index] = '\0';

		if ( strcmp(rxBuff , "t") == 0){

			APP_LOG_COLOR(GREEN);
			APP_LOG(0, 1, "\tTransmition required by the user\r\n");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "Transmition required by user");
			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
			if (SEND_BY_PUSH_BUTTON == false){
				UTIL_TIMER_Start(&TxTimer);
			}
		}
		else if ( strcmp(rxBuff , "r") == 0){
			APP_LOG_COLOR(GREEN);
			APP_LOG(0, 1, "\tThe Device is resetting...\r\n");
			NVIC_SystemReset();
		}
		else if ( strcmp(rxBuff , "help") == 0 || strcmp(rxBuff , "h") == 0){
			APP_LOG_COLOR(BLUE);
			APP_LOG(0, 1, "\t- t \t\t transmission of a new LoRaWAN packet\r\n");
			APP_LOG(0, 1, "\t- r \t\t Reset End-Device\r\n");
			APP_LOG(0, 1, "\t- h \t\t Help\r\n");
			APP_LOG(0, 1, "\t- lora \t\t Enter Raw LoRa Packet mode\r\n");

			lcd_printf(LCD_DEFAULT_FONT_COLOR, "");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "- t    Transmit new packet");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "- r    Reset");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "- h    Help");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "- lora Enter Raw LoRa mode");
		}

		else if ( strcmp(rxBuff , "lora") == 0 ){
			if( LoRaMode == 0 ){
				LoRaMode = 1;
				UTIL_TIMER_Stop(&TxTimer);
				BSP_PB_DeInit(BUTTON_SW1);
				APP_LOG_COLOR(RED);
				APP_LOG(0, 1, "\tLoRaWAN application stops\r\n");
				APP_LOG_COLOR(BLUE);
				APP_LOG(0, 1, "\r\n\r\nRaw LoRa Packet Application\r\n\r\n");
				APP_LOG_COLOR(WHITE);
				APP_LOG(0, 1, "\r\nType the following command to send a Raw LoRa Packet\r\n");
				APP_LOG(0, 1, "> Command format : LORA=Frequency:Power:SF:Payload\r\n");
				APP_LOG(0, 1, "> Example :        LORA=868100000:14:7:01020304 \r\n");

				lcd_printf(LCD_RED, "LoRaWAN application stopped");
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "");
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "Raw LoRa Packet Application");
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "Please refer to the console");
			}
			else{
				APP_LOG_COLOR(RED);
				APP_LOG(0, 1, "\r\n - You already entered the Raw LoRa Packet mode\r\n - To send a LoRa command please use this format: LORA=Frequency:Power:SF:Payload\r\n");
			}
		}
		else if(isTriggered == 1)
		{
			PrepareLoRaFrame("ERROR");
			isTriggered = 0;
			index = 0;
		}
		else if (LoRaMode == 1){
			PrepareLoRaFrame(rxBuff);
		}

		else{
			APP_LOG_COLOR(GREEN);
			APP_LOG(0, 1, "\tUnknown command\r\n");
		}
		index = 0;
	}
	else{
		rxBuff[index++] = *PData;
		if ( index == RX_BUFF_SIZE ){
			isTriggered = 1;
			index = 0;
		}
	}
	APP_LOG_COLOR(RESET_COLOR);

	//lcd_printf(LCD_DEFAULT_FONT_COLOR, "Byte reception");
	lcd_print_buf();
}

void CENTER_Pressed_Button(void){
	APP_LOG(0, 1, "Center button pressed!\r\n");
	//lcd_printf(LCD_BLUE, "Center button pressed!");
	lcd_printf(LCD_BLUE, "Forced transmition");
}

void DOWN_Pressed_Button(void){
	APP_LOG(0, 1, "Down button pressed!\r\n");
	ST7789_Fill_Color(LCD_DEFAULT_BACKGROUND);
	//lcd_printf(LCD_BLUE, "Down button pressed!");
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case  BUTTON_SW1_PIN:
		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
		break;
	//PROJECT BUTTON LCD
	case  GPIO_PIN_9:
		CENTER_Pressed_Button();
		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
		break;
	//PROJECT BUTTON LCD END
	case  GPIO_PIN_8:
		DOWN_Pressed_Button();
	case  BUTTON_SW2_PIN:
		break;
	case  BUTTON_SW3_PIN:
		break;
	default:
		break;
	}
}

//PROJECT BUTTON LCD
void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR1 & EXTI_PR1_PIF8 == EXTI_PR1_PIF8){
		lcd_printf(LCD_BLACK, "true");
	}

	if ((EXTI->PR1 & EXTI_PR1_PIF8) == EXTI_PR1_PIF8) {
		// Pin 8 is IT source
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

	}
	else if ((EXTI->PR1 & EXTI_PR1_PIF9) == EXTI_PR1_PIF9) {
		// Pin 9 is IT source
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
	}
	else __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8 | GPIO_PIN_9);

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_DisplayOnLCD), CFG_SEQ_Prio_LCD);
}
//PROJECT BUTTON LCD END

static void SendTxData(void)
{
	UTIL_TIMER_Time_t nextTxIn = 0;
	uint32_t i = 0;
	uint8_t channel = 0;

	EnvSensors_Read(&sensor_data);
	sensor_data.stm32wl_temperature = (SYS_GetTemperatureLevel() >> 8);
	sensor_data.temperature_simulated = simuTemperature();
	sensor_data.humidity_simulated = simuHumidity();
	sensor_data.atim_thaq = simuAtim();
	sensor_data.watteco_tempo = simuWatteco();
	sensor_data.tct_egreen = simuTct();

	if (WATTECO_TEMPO) AppData.Port = WATTECO_TEMPO_PORT;
	else AppData.Port = APP_PORT;

	if(PAYLOAD_1234){
		AppData.Buffer[i++] = 0x01;
		AppData.Buffer[i++] = 0x02;
		AppData.Buffer[i++] = 0x03;
		AppData.Buffer[i++] = 0x04;
//		AppData.Buffer[i++] = 'H';
//		AppData.Buffer[i++] = 'E';
//		AppData.Buffer[i++] = 'L';
//		AppData.Buffer[i++] = 'L';
//		AppData.Buffer[i++] = 'O';
		AppData.BufferSize = i;
	}
	else if(!CAYENNE_LPP){
		if(PAYLOAD_TEMPERATURE){
			if(SENSOR_ENABLED){
				AppData.Buffer[i++] = sensor_data.stts751_temperature_int8;
			}
			else{
				AppData.Buffer[i++] = sensor_data.temperature_simulated;
			}
		}
		if(PAYLOAD_HUMIDITY){
			if(SENSOR_ENABLED){
				AppData.Buffer[i++] = sensor_data.hts221_humidity_int8;
			}
			else{
				AppData.Buffer[i++] = sensor_data.humidity_simulated;
			}
		}
		if(USMB_VALVE){
			AppData.Buffer[i++] = sensor_data.setpoint;
			AppData.Buffer[i++] = sensor_data.temperature_simulated;
		}
		else if(ATIM_THAQ){
			/* Fill the buffer with each useful bytes */
			AppData.Buffer[i++] = sensor_data.atim_thaq.header1;
			AppData.Buffer[i++] = sensor_data.atim_thaq.header2;
			AppData.Buffer[i++] = sensor_data.atim_thaq.header3;
			AppData.Buffer[i++] = sensor_data.atim_thaq.header4;
			AppData.Buffer[i++] = sensor_data.atim_thaq.sensor_status>>8;
			AppData.Buffer[i++] = sensor_data.atim_thaq.sensor_status;
			AppData.Buffer[i++] = sensor_data.atim_thaq.temperature>>8;
			AppData.Buffer[i++] = sensor_data.atim_thaq.temperature;
			AppData.Buffer[i++] = sensor_data.atim_thaq.humidity>>8;
			AppData.Buffer[i++] = sensor_data.atim_thaq.humidity;
			AppData.Buffer[i++] = sensor_data.atim_thaq.air_quality>>8;
			AppData.Buffer[i++] = sensor_data.atim_thaq.air_quality;
			AppData.Buffer[i++] = sensor_data.atim_thaq.end1;
			AppData.Buffer[i++] = sensor_data.atim_thaq.end2;
			AppData.Buffer[i++] = sensor_data.atim_thaq.end3;
			AppData.Buffer[i++] = sensor_data.atim_thaq.end4;
			AppData.Buffer[i++] = sensor_data.atim_thaq.end5;
		}
		else if(WATTECO_TEMPO){
			/* Fill the buffer with each useful bytes */
			for (uint16_t w_index = 0/* Watteco index to build the frame */; w_index < 256; w_index += 8){
				AppData.Buffer[i++] = sensor_data.watteco_tempo.data[w_index/64]>>(56-(w_index%64));
			}
		} 
		else if(TCT_EGREEN){
			/* Fill the buffer with each useful bytes */
			AppData.Buffer[i++] = sensor_data.tct_egreen.type;
			AppData.Buffer[i++] = sensor_data.tct_egreen.data_type1;
			AppData.Buffer[i++] = sensor_data.tct_egreen.temperature>>8;
			AppData.Buffer[i++] = sensor_data.tct_egreen.temperature;
			AppData.Buffer[i++] = sensor_data.tct_egreen.data_type2;
			AppData.Buffer[i++] = sensor_data.tct_egreen.courant>>8;
			AppData.Buffer[i++] = sensor_data.tct_egreen.courant;
			AppData.Buffer[i++] = sensor_data.tct_egreen.data_type3;
			AppData.Buffer[i++] = sensor_data.tct_egreen.voltage>>8;
			AppData.Buffer[i++] = sensor_data.tct_egreen.voltage;
		}
		AppData.BufferSize = i;
	}
	else{	// CAYENNE
		CayenneLppReset();
		if(PAYLOAD_TEMPERATURE){
			if(SENSOR_ENABLED){
				CayenneLppAddTemperature(channel++, (int16_t)(sensor_data.hts221_temperature_float));
			}
			else{
				CayenneLppAddTemperature(channel++, sensor_data.temperature_simulated);
			}
		}

		if(PAYLOAD_HUMIDITY){
			if(SENSOR_ENABLED){
				CayenneLppAddRelativeHumidity(channel++, (int16_t)(sensor_data.hts221_humidity_float));
			}
			else{
				CayenneLppAddRelativeHumidity(channel++, (uint16_t)(sensor_data.humidity_simulated));
			}
		}
		CayenneLppCopy(AppData.Buffer);
		AppData.BufferSize = CayenneLppGetSize();
	}


	if (LORAMAC_HANDLER_SUCCESS == LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false))
	{
		/* This is written with TimeStamp ON > See function timeStampNow in sys_app.c */
		//APP_LOG(TS_ON, VLEVEL_L, " Packet transmitted\r\n");
	}
	else if (nextTxIn > 0)
	{
		APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
	}
}

uint8_t simuTemperature(void){
	static uint8_t temp=20, countUP = 0;
	static int8_t temp_valve = 40; // temp in Q7.1, so 40 = 20°C
	float error = 0;

	if(USMB_VALVE){
		/* Case thermostatic valve simulated.
		 * Temperature should reach the setpoint given by the user by adding at each function call
		 * half of the difference between the actual temperature and the setpoint.
		 * Variables are in Q7.1, so 41 = 20.5°C, from -128 to 127 => -64°C to 63.5°C*/
		error = ((float) (sensor_data.setpoint - temp_valve))/2;

		if (abs(error)<1.0 && error != 0){					// Correct the error rounding
			if (sensor_data.setpoint > temp_valve) error = 1.0;
			else error = -1.0;
		}

		temp_valve += error;
		return temp_valve;
	}
	else{
		if ( (temp == 20) || (temp == 25)){
			countUP = (countUP + 1)%2;
		}

		temp = (countUP == 1)? temp + 1 : temp - 1;
		return temp;
	}
}

uint8_t simuHumidity(void){
	static uint8_t humid = 50, countUP = 0;

	if ( (humid == 50) || (humid == 60)){
		countUP = (countUP + 1)%2;
	}

	humid = (countUP == 1)? humid + 1 : humid - 1;
	return humid;
}

/**
 * ATIM TAHQ simulated function payload 
 * @return atim payload with changing temperature
 */
atim_payload_t simuAtim(void){
	static atim_payload_t payload = {0xE0, 0x67, 0xB8, 0x3C, 0x0D08, 0x07C7, 0x0912, 0xC20C, 0x00, 0x84, 0x0D, 0x05, 0x18}; /* Random real atim_thaq payload */
	static uint8_t countUP = 0;

	payload.temperature = (countUP==0)? 0x07C7: 0x07FA; /* Temperature in centi-degree cC */
	countUP = (countUP + 1) % 2;

	return payload;
}

/**
 * TCT EGREEN simulated function payload 
 * @return tct payload with changing currant and voltage
 */
tct_payload_t simuTct(void){
	static tct_payload_t payload = {0xa0, 0x08, 0x088f, 0x0b, 0x0071, 0x0a, 0x11bf};
	static uint8_t countUP = 0;

	// Let's have a constant power P = UI
	payload.courant = (countUP==0)? 0x0071: 0x006b; /* Currant in centi-ampere cA */
	payload.voltage = (countUP==0)? 0x11bf: 0x12be; /* Voltage in mV */
	countUP = (countUP + 1) % 2;

	return payload;
}

/**
 * Watteco temp'o simulated function payload 
 * @return fixed payload
 */
watteco_payload_t simuWatteco(void){
	return (watteco_payload_t) {0x2205805533c90160, 0xb49f200903084056, 0x00b202901580ac00, 0x6491aa4e954aa512};
}

static void OnTxData(LmHandlerTxParams_t *params)
{
	if ((params != NULL))
	{
		/* Process Tx event only if it is a mcps response to prevent some internal events (mlme) */
		if (params->IsMcpsConfirm != 0)
		{
			if(params->AppData.Port != 0){
				BSP_LED_On(LED_GREEN) ;
				UTIL_TIMER_Start(&TxLedTimer);

				APP_LOG(0, 1, "- Payload     ");
				lcd_printf(LCD_RED, "______________________________________");
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "Payload");

				if(AppData.BufferSize>0)
				{
					if(USMB_VALVE){
						APP_LOG(0, 1, "Setpoint: %.1f °C | Temperature: %.1f °C", ((float) txBUFFER[0])/2, ((float) txBUFFER[1])/2);
						lcd_printf(LCD_DEFAULT_FONT_COLOR, "SetP: %.1f °C | Temp: %.1f °C", ((float) txBUFFER[0])/2, ((float) txBUFFER[1])/2);
					}
					else if(ATIM_THAQ){
						APP_LOG(0, 1, "Temperature: %.2f °C", ((float) (txBUFFER[6]<<8) + (float) txBUFFER[7])/100.0);
						lcd_printf(LCD_DEFAULT_FONT_COLOR, "Temperature: %.2f °C", ((float) (txBUFFER[6]<<8) + (float) txBUFFER[7])/100.0);
					}
					else if(WATTECO_TEMPO){ // Simply print the buffer
						for(int i=0;i<size_txBUFFER;i++){
							APP_LOG(0, 1, "%02X ", txBUFFER[i]);
						}
						lcd_printf(LCD_DEFAULT_FONT_COLOR, "WATTECO Temp'O buffer");
					}
					else if(TCT_EGREEN){
						APP_LOG(0, 1, "Currant: %.2f A | Voltage: %03u mV", ((float) (txBUFFER[5]<<8) + (float) txBUFFER[6])/100.0,
																			(txBUFFER[8]<<8) + txBUFFER[9]);
						lcd_printf(LCD_DEFAULT_FONT_COLOR, "Cur: %.2f A | Volt: %03u mV", ((float) (txBUFFER[5]<<8) + (float) txBUFFER[6])/100.0,
								(txBUFFER[8]<<8) + txBUFFER[9]);
					}
					else{
						for(int i=0;i<size_txBUFFER;i++){
							APP_LOG(0, 1, "%02X ", txBUFFER[i]);
						}
						APP_LOG(0, 1, "(hex)  |  ");

						for(int i=0;i<size_txBUFFER;i++){
							APP_LOG(0, 1, "%03u ", txBUFFER[i]);
						}
						APP_LOG(0, 1, "(dec)");
						lcd_printf(LCD_DEFAULT_FONT_COLOR, "  01 02 03 04 (dec)");
					}
				}

				APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
				APP_LOG(TS_OFF, VLEVEL_L, "- Port        %d \r\n",params->AppData.Port);
				APP_LOG(TS_OFF, VLEVEL_L, "- Fcnt        %d \r\n",params->UplinkCounter);
				APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate   %d",params->Datarate);
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "- Port        %d",params->AppData.Port);
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "- Fcnt        %d",params->UplinkCounter);
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "- Data Rate   %d", params->Datarate);

				switch(params->Datarate)
				{
					case 5 : APP_LOG(TS_OFF, VLEVEL_L, " (SF7)\r\n");break;
					case 4 : APP_LOG(TS_OFF, VLEVEL_L, " (SF8)\r\n");break;
					case 3 : APP_LOG(TS_OFF, VLEVEL_L, " (SF9)\r\n");break;
					case 2 : APP_LOG(TS_OFF, VLEVEL_L, " (SF10)\r\n");break;
					case 1 : APP_LOG(TS_OFF, VLEVEL_L, " (SF11)\r\n");break;
					case 0 : APP_LOG(TS_OFF, VLEVEL_L, " (SF12)\r\n");break;
				}

			}
			else{
				APP_LOG(TS_OFF, VLEVEL_L, "- Fcnt        %d \r\n",params->UplinkCounter);
			}
			// Print on LCD screen
			//lcd_printf(LCD_DEFAULT_FONT_COLOR, "OnTxDATA");
			lcd_print_buf();
		}
	}
}


static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
	static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };
	u_int8_t is_join = 1;

	if ((appData != NULL) || (params != NULL))
	{
		if( appData->Port == 0){
			// if port==0, it is join accept or MAC command
			//APP_LOG(TS_OFF, VLEVEL_L, "MAC Command RECEIVED\r\n");
			is_join = 0;
		}
		else{
			BSP_LED_On(LED_BLUE) ;
			UTIL_TIMER_Start(&RxLedTimer);
			APP_LOG_COLOR(BOLDBLUE);
			lcd_printf(LCD_BLUE, "________________________________");
			if (isRxConfirmed == 1){
				APP_LOG(TS_ON, VLEVEL_L, " Receiving Confirmed Data Down.\r\n");
				lcd_printf(LCD_BLUE, "Receiving Confirmed Data Down.");
			}
			else{
				APP_LOG(TS_ON, VLEVEL_L, " Receiving Unconfirmed Data Down.\r\n");
				lcd_printf(LCD_BLUE, "Receiving Unconf Data Down");
			}

			APP_LOG_COLOR(BLUE);
			APP_LOG(TS_OFF, VLEVEL_L, "- Payload     ");

			if(appData->BufferSize>0)
			{
				char * payload = "- Payload   ";
				for(int i=0 ; i<appData->BufferSize ; i++){
					APP_LOG(0, 1, "%02X ", appData->Buffer[i]);
					char val[16];
					itoa(appData->Buffer[i], val, 16);
					strcat(payload, val);
					strcat(payload, " ");
				}
				APP_LOG(0, 1, "(hex) ");
				strcat(payload, "(hex)");
				lcd_printf(LCD_BLUE, payload);
			}

			APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
			APP_LOG(TS_OFF, VLEVEL_L, "- Port        %d \r\n",appData->Port);
			APP_LOG(TS_OFF, VLEVEL_L, "- Slot        RX%s \r\n",slotStrings[params->RxSlot]);
			APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate   %d \r\n",params->Datarate);
			APP_LOG(TS_OFF, VLEVEL_L, "- RSSI        %d dBm\r\n",params->Rssi);
			APP_LOG(TS_OFF, VLEVEL_L, "- SNR         %d dB\r\n",params->Snr);
			APP_LOG_COLOR(RESET_COLOR);

			lcd_printf(LCD_BLUE, "- Port      %d",appData->Port);
			lcd_printf(LCD_BLUE, "- Slot      RX%s",slotStrings[params->RxSlot]);
			lcd_printf(LCD_BLUE, "- Data Rate %d",params->Datarate);
			lcd_printf(LCD_BLUE, "- RSSI      %d dBm",params->Rssi);
			lcd_printf(LCD_BLUE, "- SNR       %d dB",params->Snr);
		}

		switch (appData->Port)
		{
			case LORAWAN_SWITCH_CLASS_PORT:
				if (appData->BufferSize == 1)
				{
					APP_LOG_COLOR(GREEN);
					switch (appData->Buffer[0])
					{
						case 0: LmHandlerRequestClass(CLASS_A); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class A\r\n"); lcd_printf(LCD_DGREEN, "Switch to class A"); break;
						case 1: LmHandlerRequestClass(CLASS_B); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class B\r\n"); lcd_printf(LCD_DGREEN, "Switch to class B");break;
						case 2: LmHandlerRequestClass(CLASS_C); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class C\r\n"); lcd_printf(LCD_DGREEN, "Switch to class C");break;
						default: 								break;
					}
					APP_LOG_COLOR(RESET_COLOR);
				}
				break;

			case LORAWAN_USER_APP_PORT:
				if (appData->BufferSize == 1)
				{
					APP_LOG_COLOR(GREEN);
					AppLedStateOn = appData->Buffer[0] & 0x01;
					if (AppLedStateOn == RESET)
					{
						APP_LOG(TS_OFF, VLEVEL_L, "LED 3 (RED) goes OFF\r\n");
						lcd_printf(LCD_DGREEN, "LED 3 (RED) goes OFF");
						BSP_LED_Off(LED_RED) ;
					}
					else
					{
						APP_LOG(TS_OFF, VLEVEL_L, "LED 3 (RED) goes ON\r\n");
						lcd_printf(LCD_DGREEN, "LED 3 (RED) goes ON");
						BSP_LED_On(LED_RED) ;
					}
					APP_LOG_COLOR(RESET_COLOR);
				}
				break;

			case VALVE_APP_PORT:
				if (appData->BufferSize == 1)
				{
					APP_LOG_COLOR(GREEN);
					sensor_data.setpoint = appData->Buffer[0];
					APP_LOG(TS_OFF, VLEVEL_L, "New setpoint is set to %.1f °C\r\n", (float) sensor_data.setpoint/2);
					APP_LOG_COLOR(RESET_COLOR);
					lcd_printf(LCD_DGREEN, "New setpoint set to %.1f °C", (float) sensor_data.setpoint/2);
				}
				break;

			default:  break;
		}

		if (!is_join) {
			// Display logs on screen
			//lcd_printf(LCD_DEFAULT_FONT_COLOR, "OnRxDATA");
			lcd_print_buf();
		}
	}
}
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
	if (joinParams != NULL)
	{
		if (joinParams->Status == LORAMAC_HANDLER_SUCCESS)
		{
			UTIL_TIMER_Stop(&JoinLedTimer);
			BSP_LED_Off(LED_RED) ;

			APP_LOG_COLOR(GREEN);
			if (joinParams->Mode == ACTIVATION_TYPE_ABP)
			{
				APP_LOG(TS_OFF, VLEVEL_L, "\r\n> ABP Activation mode\r\n");
				lcd_printf(LCD_DGREEN, "ABP Activation Mode");
				lcd_printf(LCD_DEFAULT_FONT_COLOR, "");
			}
			else
			{
				APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOINED = OTAA!\r\n");
				lcd_printf(LCD_DGREEN, "> JOINED = OTAA");
			}


			if(SEND_BY_PUSH_BUTTON == true){
				APP_LOG(0, 1, "> Packets will be sent on a Push Button event (B1) \r\n");
				lcd_printf(LCD_DGREEN, "> Packets send on PB");
			}
			else{
				APP_LOG(0, 1, "> Packets will be sent every %d ms OR on a Push Button event (B1) \r\n", ADMIN_TxDutyCycleTime);
				lcd_printf(LCD_DGREEN, "> Packets every %ds or PB", ADMIN_TxDutyCycleTime/1000);
			}

			APP_LOG_COLOR(RESET_COLOR);

			/* Create TIMER for sending next Tx Frame  */
			// if (EventType == TX_ON_TIMER)
			/*  if (SEND_BY_PUSH_BUTTON == true)
       {
    	 BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);		// BUTTON_SW1 = PA0, IRQ number = EXTI0_IRQn

       }*/
			/* Send every time button is pushed */
			if (SEND_BY_PUSH_BUTTON == false)
			{
				UTIL_TIMER_Create(&TxTimer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
				UTIL_TIMER_SetPeriod(&TxTimer,  APP_TX_DUTYCYCLE);
				UTIL_TIMER_Start(&TxTimer);
			}

			// Send a first frame just after the Join (When using timer event to send packets)
			if(SEND_BY_PUSH_BUTTON == false){
				SendTxData();
			}
		}
		else
		{
			APP_LOG_COLOR(RED);
			APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOIN FAILED...\r\n");

			lcd_printf(LCD_RED, "> JOIN FAILED...");
			lcd_printf(LCD_DEFAULT_FONT_COLOR, "");
			// Display logs on screen
			//lcd_printf(LCD_DEFAULT_FONT_COLOR, "OnJoinRequest, failed");
			lcd_print_buf();

			LmHandlerJoin(ActivationType);
			APP_LOG_COLOR(RESET_COLOR);
			APP_LOG(0, 1, " \r\n");
		}
	}
}

static void OnMacProcessNotify(void)
{

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);

}


static void OnTxTimerEvent(void *context)
{
	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

	/*Wait for next tx slot*/
	UTIL_TIMER_Start(&TxTimer);
}

static void OnTxTimerLedEvent(void *context)
{
	BSP_LED_Off(LED_GREEN) ;
}

static void OnRxTimerLedEvent(void *context)
{
	BSP_LED_Off(LED_BLUE) ;
}

static void OnJoinTimerLedEvent(void *context)
{
	BSP_LED_Toggle(LED_RED) ;
}

/**
  * @brief Interruption button Initialization Function
  * @param None
  * @retval None
  */
static void MX_BP_IT_Init(void)
{
	//PROJECT BUTTON LCD
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	 /*Configure GPIO pin : PA9, Center button */
	 GPIO_InitStruct.Pin = GPIO_PIN_9;
	 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	 /* EXTI interrupt init*/
	 HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 1);
	 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	//PROJECT BUTTON LCD END

	 __HAL_RCC_GPIOB_CLK_ENABLE();
	 /*Configure GPIO pin : PA8, Down button */
	 GPIO_InitStruct.Pin = GPIO_PIN_8;
	 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	 /* EXTI interrupt init*/
	 HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	 HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
