
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

#include  "General_Setup.h"
uint8_t simuTemperature(void);
static void byteReception(uint8_t *PData, uint16_t Size, uint8_t Error);
#define RX_BUFF_SIZE 60
static uint8_t rxBuff[RX_BUFF_SIZE];
uint8_t isRxConfirmed;
uint32_t LoRaMode = 0;


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

static TxEventType_t EventType = ADMIN_TX_TYPE;
static UTIL_TIMER_Object_t TxTimer;
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
static uint8_t AppLedStateOn = RESET;
static UTIL_TIMER_Object_t TxLedTimer;
static UTIL_TIMER_Object_t RxLedTimer;
static UTIL_TIMER_Object_t JoinLedTimer;

void LoRaWAN_Init(void)
{
	// Starts the RX USART2 process by interrupt
	UTIL_ADV_TRACE_StartRxProcess(byteReception);

	APP_LOG_COLOR(CLEAR_SCREEN);
	APP_LOG_COLOR(RESET_COLOR);
	APP_LOG(0, 1, " \r\n");
    APP_LOG(0, 1, "########################################\r\n");
    APP_LOG(0, 1, "###### LoRaWAN Training Session ########\r\n");
    APP_LOG(0, 1, "###### Savoie Mont Blanc University ####\r\n");
    APP_LOG(0, 1, " \r\n");

    BSP_LED_Init(LED_BLUE);
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_RED);

	BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);		// BUTTON_SW1 = PA0, IRQ number = EXTI0_IRQn
	BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_GPIO);

/****** Raw LoRa Packet Application *************/
	if ( BSP_PB_GetState(BUTTON_SW2) == 0 ) {

			LoRaMode = 1;
			APP_LOG_COLOR(BLUE);
			APP_LOG(0, 1, "Raw LoRa Packet Application\r\n\r\n");

			APP_LOG_COLOR(WHITE);
			APP_LOG(0, 1, "Type the following command to send a Raw LoRa Packet\r\n");
			APP_LOG(0, 1, "> Command format : LORA=Frequency:Power:SF:Payload\r\n");
			APP_LOG(0, 1, "> Example :        LORA=868100000:14:7:48454C4C4F \r\n");
	}

/***** LoRaWAN Standalone Application  ***********/
	else{
			APP_LOG(0, 1, "> Activation mode         %s",(ACTIVATION_MODE == ABP) ? "ABP \r\n" : "OTAA \r\n");
			if(CLASS == CLASS_A){
				APP_LOG(0, 1, "> Class                   A\r\n");
			}else if(CLASS == CLASS_B){
					  APP_LOG(0, 1, "> Class                   B\r\n");
					}
					else if(CLASS == CLASS_C){
							  APP_LOG(0, 1, "> Class                   C\r\n");
							}
			if(SEND_BY_PUSH_BUTTON == true){
				APP_LOG(0, 1, "> Send frame              On push button event \r\n");
			}
			else{
				APP_LOG(0, 1, "> Send frame every        %d ms\r\n", ADMIN_TxDutyCycleTime);
			}
			APP_LOG(0, 1, "> Spreading Factor        %d \r\n", SPREADING_FACTOR);
			APP_LOG(0, 1, "> Adaptive Data Rate      %s", (ADAPTIVE_DR == true) ? "ON \r\n" : "OFF \r\n");
			APP_LOG(0, 1, "> Uplink Frame            %s",(CONFIRMED == true) ? "Confirmed\r\n" : "Unconfirmed\r\n");
			APP_LOG(0, 1, "> App Port number         %d \r\n", PORT);

			if(PAYLOAD_TEMPERATURE == true){
				APP_LOG(0, 1, "> Payload content         1-byte temperature\r\n");
			}
			else if(PAYLOAD_HELLO == true){
				APP_LOG(0, 1, "> Payload content:        String HELLO\r\n");
			}
			else if(CAYENNE_LPP == true){
				APP_LOG(0, 1, "> Payload content:        CayenneLPP, sensors\r\n");
			   }
			//APP_LOG(0, 1, "> Low Power:              %s",(LOW_POWER == true) ? "ON \r\n" : "OFF \r\n");
			APP_LOG(0, 1, "\r\n");
		   APP_LOG_COLOR(BLUE);

		  UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
		  UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
		  UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
		  UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
		  UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
		  UTIL_TIMER_SetPeriod(&JoinLedTimer, 1000);

		  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
		  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);

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
			APP_LOG(0, 1, "\tTransmition required the by user\r\n");
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
		}

		else if ( strcmp(rxBuff , "lora") == 0 ){
			if( LoRaMode == 0 ){
				LoRaMode = 1;
				UTIL_TIMER_Stop(&TxTimer);
				BSP_PB_DeInit(BUTTON_SW1);
				APP_LOG_COLOR(RED);
				APP_LOG(0, 1, "\tLoRaWAN application stops - Enter Raw LoRa Packet mode\r\n");
				APP_LOG_COLOR(WHITE);
			    APP_LOG(0, 1, "\r\nType the following command to send a Raw LoRa Packet\r\n");
			    APP_LOG(0, 1, "> Command format : LORA=Frequency:Power:SF:Payload\r\n");
			    APP_LOG(0, 1, "> Example :        LORA=868100000:14:7:48454C4C4F \r\n");
			}
			else{
				APP_LOG_COLOR(RED);
				APP_LOG(0, 1, "\r\n - You already entered the Raw LoRa Packet mode\r\n - To send a LoRa command please use this format: LORA=Frequency:Power:SF:Payload\r\n");
			}
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
			index = 0;
		}
	}
	APP_LOG_COLOR(RESET_COLOR);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUTTON_SW1_PIN:
      UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);
      break;
    case  BUTTON_SW2_PIN:
      break;
    case  BUTTON_SW3_PIN:
      break;
    default:
      break;
  }
}

static void SendTxData(void)
{
  uint16_t pressure = 0;
  int16_t temperature = 0;
  sensor_t sensor_data;
  UTIL_TIMER_Time_t nextTxIn = 0;

  uint8_t channel = 0;

  EnvSensors_Read(&sensor_data);
  temperature = (SYS_GetTemperatureLevel() >> 8);
  pressure    = (uint16_t)(sensor_data.pressure * 100 / 10);      /* in hPa / 10 */

  AppData.Port = LORAWAN_USER_APP_PORT;

  if(ADMIN_PAYLOAD_TEMPERATURE)
  {
	  //temperature_20_25 ();						// change temp value
	  AppData.Port = LORAWAN_USER_APP_PORT;
	  uint32_t i = 0;
	  AppData.Buffer[i++] = simuTemperature();
	  AppData.BufferSize = i;
  }
  else if(ADMIN_PAYLOAD_HELLO)
  {
	  AppData.Port = LORAWAN_USER_APP_PORT;

	  uint32_t i = 0;
	  AppData.Buffer[i++] = 'H';
	  AppData.Buffer[i++] = 'E';
	  AppData.Buffer[i++] = 'L';
	  AppData.Buffer[i++] = 'L';
	  AppData.Buffer[i++] = 'O';
	  AppData.BufferSize = i;
  }
  else if(ADMIN_CAYENNE)
  {
	CayenneLppReset();
	//CayenneLppAddBarometricPressure(channel++, pressure);
	CayenneLppAddTemperature(channel++, temperature);
	CayenneLppAddRelativeHumidity(channel++, (uint16_t)(sensor_data.humidity));
	//CayenneLppAddAnalogInput(channel++, GetBatteryLevel()*33/255);
	//CayenneLppAddDigitalInput(channel++, GetBatteryLevel());
	CayenneLppAddDigitalOutput(channel++, AppLedStateOn);
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
	static uint8_t temp = 20, countUP = 0;

	if ( (temp == 20) || (temp == 25)){
		countUP = (countUP + 1)%2;
	}

	temp = (countUP == 1)? temp + 1 : temp - 1;
	return temp;
}


static void OnTxData(LmHandlerTxParams_t *params)
{
  if ((params != NULL))
  {
    /* Process Tx event only if its a mcps response to prevent some internal events (mlme) */
    if (params->IsMcpsConfirm != 0)
    {
    	if(params->AppData.Port != 0){
    		  BSP_LED_On(LED_GREEN) ;
			  UTIL_TIMER_Start(&TxLedTimer);

			 /* if (CONFIRMED == true){
				  APP_LOG_COLOR(GREEN);
				  APP_LOG(0, 1, " ACK Received \r\n");
				  APP_LOG_COLOR(RESET_COLOR);
			  }*/

			  APP_LOG(0, 1, "- Payload     ");
			  if(AppData.BufferSize>0)
			  {
				  for(int i=0;i<AppData.BufferSize;i++){
					  APP_LOG(0, 1, "%02X ", AppData.Buffer[i]);
				  }
				  APP_LOG(0, 1, "(hex)  |  ");

				  for(int i=0;i<AppData.BufferSize;i++){
				  	APP_LOG(0, 1, "%03u ", AppData.Buffer[i]);
				  }
				 	APP_LOG(0, 1, "(dec)");
			  }

			  APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
			  APP_LOG(TS_OFF, VLEVEL_L, "- Port        %d \r\n",params->AppData.Port);
			  APP_LOG(TS_OFF, VLEVEL_L, "- Fcnt        %d \r\n",params->UplinkCounter);
			  APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate   %d",params->Datarate);
			  switch(params->Datarate)
				 {
				  case 5 : APP_LOG(TS_OFF, VLEVEL_L, " (SF7)\r\n");break;
				  case 4 : APP_LOG(TS_OFF, VLEVEL_L, " (SF8)\r\n");break;
				  case 3 : APP_LOG(TS_OFF, VLEVEL_L, " (SF9)\r\n");break;
				  case 2 : APP_LOG(TS_OFF, VLEVEL_L, " (SF10)\r\n");break;
				  case 1 : APP_LOG(TS_OFF, VLEVEL_L, " (SF11)\r\n");break;
				  case 0 : APP_LOG(TS_OFF, VLEVEL_L, " (SF12)\r\n");break;
				 }
			  //APP_LOG(TS_OFF, VLEVEL_L, "- TX Power:   %d\r\n",params->TxPower);
    	}
    	else{
    		//APP_LOG_COLOR(GREEN);
    		//APP_LOG(TS_OFF, VLEVEL_L, "  \r\nMAC COMMAND SENT (no data)\r\n");
    		APP_LOG(TS_OFF, VLEVEL_L, "- Fcnt        %d \r\n",params->UplinkCounter);
    		//APP_LOG(TS_OFF, VLEVEL_L, "- Port        %d \r\n",params->AppData.Port);
    		//APP_LOG_COLOR(RESET_COLOR);
    		// A MAC Command has been transmitted
    		// This is to fix the LOG issue when the first frame is sent in ABP. It seems that the Device wants to send a frame with port=0
    		// /!\ When using ABP on TTN, the FCnt = 2 is MAC command, index, a frame is sent with the following Frame Payload = 0703070307030703070308
    		// 07 is a new Channel Req Mac command. 03 Means that it had been accepted.
    		// 08 is a RXTimingSetupReq Mac command. (No payload for the response).
    	}
    }
  }
}


static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

  if ((appData != NULL) || (params != NULL))
  {
	  if( appData->Port == 0){
		  // if port==0, it is join accept or MAC command
		  //APP_LOG(TS_OFF, VLEVEL_L, "MAC Command RECEIVED\r\n");
	  }
	  else{
		  BSP_LED_On(LED_BLUE) ;
		  UTIL_TIMER_Start(&RxLedTimer);
		  APP_LOG_COLOR(BOLDBLUE);
		  if (isRxConfirmed == 1){
			  APP_LOG(TS_ON, VLEVEL_L, " Receiving Confirmed Data Down.\r\n");
		  }
		  else{
			  APP_LOG(TS_ON, VLEVEL_L, " Receiving Unconfirmed Data Down.\r\n");
		  }

		  APP_LOG_COLOR(BLUE);
		  APP_LOG(TS_OFF, VLEVEL_L, "- Payload     ");

		  if(appData->BufferSize>0)
			{
				for(int i=0 ; i<appData->BufferSize ; i++){
				  APP_LOG(0, 1, "%02X ", appData->Buffer[i]);
				}
				APP_LOG(0, 1, "(hex) ");
			}

		  APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
		  APP_LOG(TS_OFF, VLEVEL_L, "- Port        %d \r\n",appData->Port);
		  APP_LOG(TS_OFF, VLEVEL_L, "- Slot        RX%s \r\n",slotStrings[params->RxSlot]);
		  APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate   %d \r\n",params->Datarate);
		  APP_LOG(TS_OFF, VLEVEL_L, "- RSSI        %d dBm\r\n",params->Rssi);
		  APP_LOG(TS_OFF, VLEVEL_L, "- SNR         %d dB\r\n",params->Snr);
		  APP_LOG_COLOR(RESET_COLOR);
	  }

	switch (appData->Port)
	{
	  case LORAWAN_SWITCH_CLASS_PORT:
		if (appData->BufferSize == 1)
		{
		  APP_LOG_COLOR(GREEN);
		  switch (appData->Buffer[0])
		  {
			case 0: LmHandlerRequestClass(CLASS_A); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class A\r\n"); break;
			case 1: LmHandlerRequestClass(CLASS_B); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class B\r\n"); break;
			case 2: LmHandlerRequestClass(CLASS_C); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class C\r\n"); break;
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
			BSP_LED_Off(LED_RED) ;
		  }
		  else
		  {
			APP_LOG(TS_OFF, VLEVEL_L, "LED 3 (RED) goes ON\r\n");
			BSP_LED_On(LED_RED) ;
		  }
		  APP_LOG_COLOR(RESET_COLOR);
		}
		break;

	  default:  break;
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
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOINED = OTAA !\r\n");
      }


	  if(SEND_BY_PUSH_BUTTON == true){
		  APP_LOG(0, 1, "> Packets will be sent on a Push Button event (B1) \r\n");
	  }
	  else{
		  APP_LOG(0, 1, "> Packets will be sent every %d ms OR on a Push Button event (B1) \r\n", ADMIN_TxDutyCycleTime);
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
      APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOIN FAILED ...\r\n");
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

