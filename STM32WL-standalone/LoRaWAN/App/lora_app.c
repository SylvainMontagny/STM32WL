/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_app.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
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

/* USER CODE BEGIN Includes */

#include  "General_Setup.h"

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief LoRa State Machine states
  */
typedef enum TxEventType_e
{
  /**
    * @brief Appdata Transmission issue based on timer every TxDutyCycleTime
    */
  TX_ON_TIMER,
  /**
    * @brief Appdata Transmission external event plugged on OnSendEvent( )
    */
  TX_ON_EVENT
  /* USER CODE BEGIN TxEventType_t */

  /* USER CODE END TxEventType_t */
} TxEventType_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

uint8_t temp = 20;
uint8_t hl = 1;

void temperature_20_25 (){

	if (temp <= 20){
		hl = 1;
	}
	else if (temp >=25){
		hl = 0;
	}
	else{
		hl=hl;
	}

	if(hl==1){
		temp ++;
	}
	else{
		temp --;
	}

}



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  LoRa End Node send request
  */
static void SendTxData(void);

/**
  * @brief  TX timer callback function
  * @param  context ptr of timer context
  */
static void OnTxTimerEvent(void *context);

/**
  * @brief  join event callback function
  * @param  joinParams status of join
  */
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);

/**
  * @brief  tx event callback function
  * @param  params status of last Tx
  */
static void OnTxData(LmHandlerTxParams_t *params);

/**
  * @brief callback when LoRa application has received a frame
  * @param appData data received in the last Rx
  * @param params status of last Rx
  */
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/*!
 * Will be called each time a Radio IRQ is handled by the MAC layer
 *
 */
static void OnMacProcessNotify(void);

/* USER CODE BEGIN PFP */

/**
  * @brief  LED Tx timer callback function
  * @param  context ptr of LED context
  */
static void OnTxTimerLedEvent(void *context);

/**
  * @brief  LED Rx timer callback function
  * @param  context ptr of LED context
  */
static void OnRxTimerLedEvent(void *context);

/**
  * @brief  LED Join timer callback function
  * @param  context ptr of LED context
  */
static void OnJoinTimerLedEvent(void *context);

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
static ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

/**
  * @brief LoRaWAN handler Callbacks
  */
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

/**
  * @brief LoRaWAN handler parameters
  */
static LmHandlerParams_t LmHandlerParams =
{
  .ActiveRegion =             ACTIVE_REGION,
  .DefaultClass =             LORAWAN_DEFAULT_CLASS,
  .AdrEnable =                LORAWAN_ADR_STATE,
  .TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
  .PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};

/**
  * @brief Type of Event to generate application Tx
  */
// static TxEventType_t EventType = TX_ON_EVENT;
static TxEventType_t EventType = ADMIN_TX_TYPE;
//static TxEventType_t EventType = TX_ON_TIMER;
/**
  * @brief Timer to handle the application Tx
  */
static UTIL_TIMER_Object_t TxTimer;

/* USER CODE BEGIN PV */
/**
  * @brief User application buffer
  */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/**
  * @brief User application data structure
  */
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

/**
  * @brief Specifies the state of the application LED
  */
static uint8_t AppLedStateOn = RESET;

/**
  * @brief Timer to handle the application Tx Led to toggle
  */
static UTIL_TIMER_Object_t TxLedTimer;

/**
  * @brief Timer to handle the application Rx Led to toggle
  */
static UTIL_TIMER_Object_t RxLedTimer;

/**
  * @brief Timer to handle the application Join Led to toggle
  */
static UTIL_TIMER_Object_t JoinLedTimer;

/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */

/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_1 */

	APP_LOG(0, 1, " \r\n");
    APP_LOG(0, 1, "########################################\r\n");
    APP_LOG(0, 1, "###### LoRaWAN Training Session ########\r\n");
    APP_LOG(0, 1, "###### Savoie Mont Blanc University ####\r\n");
    APP_LOG(0, 1, " \r\n");


    if(SEND_BY_PUSH_BUTTON == true){
        APP_LOG(0, 1, "> Send frame :            On push button event \r\n", ADMIN_TxDutyCycleTime);
    }
    else{
    APP_LOG(0, 1, "> Send frame every:       %d ms\r\n", ADMIN_TxDutyCycleTime);
    }

    APP_LOG(0, 1, "> Spreading Factor:       %d \r\n", SPREADING_FACTOR);

    if(ADAPTIVE_DR == 1){
    	APP_LOG(0, 1, "> Adaptive Data Rate:     ON \r\n");
    }
    else{
    	APP_LOG(0, 1, "> Adaptive Data Rate:     OFF \r\n");
    }

    if(CONFIRMED == 1){
    	APP_LOG(0, 1, "> Frame:                  Confirmed\r\n");
    }
    else{
    	APP_LOG(0, 1, "> Frame:                  Unconfirmed\r\n");
    }

    APP_LOG(0, 1, "> App Port number:        %d \r\n", PORT);


    // ENABLE_TEMPERATURE >>> PAYLOAD_TEMPERATURE
    // PAYLOAD_HELLO
    if(PAYLOAD_TEMPERATURE == 1){
    	APP_LOG(0, 1, "> Payload content:        1-byte temperature\r\n");
    }
    else if(PAYLOAD_HELLO == 1){
    	APP_LOG(0, 1, "> Payload content:        String HELLO\r\n");
    }
    else if(CAYENNE_LPP == 1){
       	APP_LOG(0, 1, "> Payload content:        CayenneLPP, sensors\r\n");
       }

    if( LOW_POWER == 1){
    	APP_LOG(0, 1, "> Low Power:              ON \r\n");
    }
    else{
    	APP_LOG(0, 1, "> Low Power:              OFF \r\n");
    }

    APP_LOG(0, 1, "\r\n");

    if(ACTIVATION_MODE == ABP){
    	APP_LOG(0, 1, "> Activation mode :       ABP \r\n");
    }
    else{
    	APP_LOG(0, 1, "> Activation mode :       OTAA \r\n");
    }

  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);

  /* Get LoRa APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION:        V%X.%X.%X\r\n",
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW LoraWAN info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

  UTIL_TIMER_Create(&TxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
  UTIL_TIMER_Create(&RxLedTimer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
  UTIL_TIMER_Create(&JoinLedTimer, 0xFFFFFFFFU, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);
  UTIL_TIMER_SetPeriod(&TxLedTimer, 500);
  UTIL_TIMER_SetPeriod(&RxLedTimer, 500);
  UTIL_TIMER_SetPeriod(&JoinLedTimer, 500);

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);
  /* Init Info table used by LmHandler*/
  LoraInfo_Init();

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks);
  LmHandlerConfigure(&LmHandlerParams);

  UTIL_TIMER_Start(&JoinLedTimer);

  LmHandlerJoin(ActivationType);

  if (EventType == TX_ON_TIMER)
  {
    /* send every time timer elapses */
    UTIL_TIMER_Create(&TxTimer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);
    //UTIL_TIMER_Create(&TxTimer,  0xFFFFFFFFU,  UTIL_TIMER_PERIODIC, OnTxTimerEvent, NULL);
    UTIL_TIMER_SetPeriod(&TxTimer,  APP_TX_DUTYCYCLE);
    UTIL_TIMER_Start(&TxTimer);
  }
  else
  {
    /* send every time button is pushed */
    BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);
  }
}

/* USER CODE BEGIN PB_Callbacks */
/* Note: Current the stm32wlxx_it.c generated by STM32CubeMX does not support BSP for PB in EXTI mode. */
/* In order to get a push button IRS by code automatically generated */
/* HAL_GPIO_EXTI_Callback is today the only available possibility. */
/* Using HAL_GPIO_EXTI_Callback() shortcuts the BSP. */
/* If users wants to go through the BSP, stm32wlxx_it.c should be updated  */
/* in the USER CODE SESSION of the correspondent EXTIn_IRQHandler() */
/* to call the BSP_PB_IRQHandler() or the HAL_EXTI_IRQHandler(&H_EXTI_n);. */
/* Then the below HAL_GPIO_EXTI_Callback() can be replaced by BSP callback */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUTTON_SW1_PIN:
      /* Note: when "EventType == TX_ON_TIMER" this GPIO is not initialized */
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

/* USER CODE END PB_Callbacks */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
  if ((appData != NULL) || (params != NULL))
  {
    BSP_LED_On(LED_BLUE) ;

    UTIL_TIMER_Start(&RxLedTimer);

    static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

    if (appData->Port == 0){
    	// if port==0, it is join accept or MAC command
    	//APP_LOG(TS_OFF, VLEVEL_L, "MAC Command RECEIVED\r\n");
    }
    else{
		APP_LOG(TS_OFF, VLEVEL_L, "FRAME RECEIVED: ");
		if(appData->Port == ADMIN_USER_APP_PORT) {
			if (appData->Buffer[0] == 0){
				APP_LOG(TS_OFF, VLEVEL_L, "LED 3 (RED) goes OFF");
			}
			if (appData->Buffer[0] == 1){
				APP_LOG(TS_OFF, VLEVEL_L, "LED 3 (RED) goes ON");
			}
		}
		APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
		APP_LOG(TS_OFF, VLEVEL_L, "- Payload:    ");
		if(appData->BufferSize>0)
		{
			for(int i=0;i<appData->BufferSize;i++){
			  APP_LOG(TS_OFF, VLEVEL_L, "%x ", appData->Buffer[i]); //%2x ?
			}
		}

		APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
		APP_LOG(TS_OFF, VLEVEL_L, "- Port:       %d \r\n",appData->Port);
		APP_LOG(TS_OFF, VLEVEL_L, "- Slot:       RX%s \r\n",slotStrings[params->RxSlot]);
		APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate:  %d \r\n",params->Datarate);
		APP_LOG(TS_OFF, VLEVEL_L, "- RSSI:       %d dBm\r\n",params->Rssi);
		APP_LOG(TS_OFF, VLEVEL_L, "- SNR:        %d dB\r\n",params->Snr);
    }

    APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Indication ==========\r\n");
    APP_LOG(TS_OFF, VLEVEL_H, "###### D/L FRAME:%04d | SLOT:%s | PORT:%d | DR:%d | RSSI:%d | SNR:%d\r\n",
            params->DownlinkCounter, slotStrings[params->RxSlot], appData->Port, params->Datarate, params->Rssi, params->Snr);

    APP_LOG(TS_OFF, VLEVEL_H, "###### PAYLOAD: ");
    if(appData->BufferSize>0)
    {
        for(int i=0;i<appData->BufferSize;i++){
      	  APP_LOG(TS_OFF, VLEVEL_H, "%x ", appData->Buffer[i]); //%2x ?
        }
    }
    APP_LOG(TS_OFF, VLEVEL_H, "\r\n");

    // print key
    //SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    //Key_t *keyItem;
    //retval = SecureElementGetKeyByID(MC_ROOT_KEY, &keyItem);
    //MW_LOG(TS_OFF, VLEVEL_M, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", HEX16(keyItem->KeyValue));
    // end print key


    APP_LOG(TS_OFF, VLEVEL_H, "\r\n");

    switch (appData->Port)
    {
      case LORAWAN_SWITCH_CLASS_PORT:
        if (appData->BufferSize == 1)
        {
          switch (appData->Buffer[0])
          {
            case 0:
            {
              LmHandlerRequestClass(CLASS_A);
              break;
            }
            case 1:
            {
              LmHandlerRequestClass(CLASS_B);
              break;
            }
            case 2:
            {
              LmHandlerRequestClass(CLASS_C);
              break;
            }
            default:
              break;
          }
        }
        break;
      case LORAWAN_USER_APP_PORT:
        if (appData->BufferSize == 1)
        {
          AppLedStateOn = appData->Buffer[0] & 0x01;
          if (AppLedStateOn == RESET)
          {
            APP_LOG(TS_OFF, VLEVEL_H,   "LED OFF\r\n");
            BSP_LED_Off(LED_RED) ;
          }
          else
          {
            APP_LOG(TS_OFF, VLEVEL_H, "LED ON\r\n");
            BSP_LED_On(LED_RED) ;
          }
        }
        break;

      default:

        break;
    }
  }
  /* USER CODE END OnRxData_1 */
}

static void SendTxData(void)
{
  /* USER CODE BEGIN SendTxData_1 */
  uint16_t pressure = 0;
  int16_t temperature = 0;
  sensor_t sensor_data;
  UTIL_TIMER_Time_t nextTxIn = 0;

#ifdef CAYENNE_LPP
  uint8_t channel = 0;
#else
  uint16_t humidity = 0;
  uint32_t i = 0;
  int32_t latitude = 0;
  int32_t longitude = 0;
  uint16_t altitudeGps = 0;
#endif /* CAYENNE_LPP */

  EnvSensors_Read(&sensor_data);
  temperature = (SYS_GetTemperatureLevel() >> 8);
  pressure    = (uint16_t)(sensor_data.pressure * 100 / 10);      /* in hPa / 10 */

  AppData.Port = LORAWAN_USER_APP_PORT;

#ifdef CAYENNE_LPP

  if(ADMIN_PAYLOAD_TEMPERATURE)
  {
	  temperature_20_25 ();						// change temp value

	  AppData.Port = LORAWAN_USER_APP_PORT;

	  uint32_t i = 0;
	  AppData.Buffer[i++] = temp;
	  AppData.BufferSize = i;

  }

  // Ici a la place de APP_PERSO mettre PAYLOAD_HELLO
  // Modifier le bufffer par le string "hello".
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
  else
  {
	CayenneLppReset();
	CayenneLppAddBarometricPressure(channel++, pressure);
	CayenneLppAddTemperature(channel++, temperature);
	CayenneLppAddRelativeHumidity(channel++, (uint16_t)(sensor_data.humidity));

	if ((LmHandlerParams.ActiveRegion != LORAMAC_REGION_US915) && (LmHandlerParams.ActiveRegion != LORAMAC_REGION_AU915)
	  && (LmHandlerParams.ActiveRegion != LORAMAC_REGION_AS923))
	{
	CayenneLppAddDigitalInput(channel++, GetBatteryLevel());
	CayenneLppAddDigitalOutput(channel++, AppLedStateOn);
	}

	CayenneLppCopy(AppData.Buffer);
	AppData.BufferSize = CayenneLppGetSize();
  }


#else  /* not CAYENNE_LPP */




  humidity    = (uint16_t)(sensor_data.humidity * 10);            /* in %*10     */

  AppData.Buffer[i++] = AppLedStateOn;
  AppData.Buffer[i++] = (uint8_t)((pressure >> 8) & 0xFF);
  AppData.Buffer[i++] = (uint8_t)(pressure & 0xFF);
  AppData.Buffer[i++] = (uint8_t)(temperature & 0xFF);
  AppData.Buffer[i++] = (uint8_t)((humidity >> 8) & 0xFF);
  AppData.Buffer[i++] = (uint8_t)(humidity & 0xFF);

  if ((LmHandlerParams.ActiveRegion == LORAMAC_REGION_US915) || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AU915)
      || (LmHandlerParams.ActiveRegion == LORAMAC_REGION_AS923))
  {
    AppData.Buffer[i++] = 0;
    AppData.Buffer[i++] = 0;
    AppData.Buffer[i++] = 0;
    AppData.Buffer[i++] = 0;
  }
  else
  {
    latitude = sensor_data.latitude;
    longitude = sensor_data.longitude;

    AppData.Buffer[i++] = GetBatteryLevel();        /* 1 (very low) to 254 (fully charged) */
    AppData.Buffer[i++] = (uint8_t)((latitude >> 16) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)((latitude >> 8) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(latitude & 0xFF);
    AppData.Buffer[i++] = (uint8_t)((longitude >> 16) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)((longitude >> 8) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(longitude & 0xFF);
    AppData.Buffer[i++] = (uint8_t)((altitudeGps >> 8) & 0xFF);
    AppData.Buffer[i++] = (uint8_t)(altitudeGps & 0xFF);
  }

  AppData.BufferSize = i;


#endif /* CAYENNE_LPP */

  if (LORAMAC_HANDLER_SUCCESS == LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false))
  {
	// APP_LOG(TS_ON, VLEVEL_L, "SEND REQUEST\r\n");
	APP_LOG(TS_OFF, VLEVEL_L, "\r\nFRAME SENT\r\n");
  }
  else if (nextTxIn > 0)
  {
    APP_LOG(TS_ON, VLEVEL_L, "Next Tx in  : ~%d second(s)\r\n", (nextTxIn / 1000));
  }

  /* USER CODE END SendTxData_1 */
}

static void OnTxTimerEvent(void *context)
{
  /* USER CODE BEGIN OnTxTimerEvent_1 */

  /* USER CODE END OnTxTimerEvent_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

  /*Wait for next tx slot*/
  UTIL_TIMER_Start(&TxTimer);
  /* USER CODE BEGIN OnTxTimerEvent_2 */

  /* USER CODE END OnTxTimerEvent_2 */
}

/* USER CODE BEGIN PrFD_LedEvents */
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

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
  if ((params != NULL))
  {
    /* Process Tx event only if its a mcps response to prevent some internal events (mlme) */
    if (params->IsMcpsConfirm != 0)
    {
      BSP_LED_On(LED_GREEN) ;
      UTIL_TIMER_Start(&TxLedTimer);

      APP_LOG(TS_OFF, VLEVEL_L, "- Payload:    ");
      if(AppData.BufferSize>0)
      {
		  for(int i=0;i<AppData.BufferSize;i++){
			  APP_LOG(TS_OFF, VLEVEL_L, "%x ", AppData.Buffer[i]);
		  }
      }

      APP_LOG(TS_OFF, VLEVEL_L, "\r\n");
      APP_LOG(TS_OFF, VLEVEL_L, "- Port:       %d \r\n",params->AppData.Port);
      APP_LOG(TS_OFF, VLEVEL_L, "- Fcnt:       %d \r\n",params->UplinkCounter);
      APP_LOG(TS_OFF, VLEVEL_L, "- Data Rate:  %d",params->Datarate);
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


      APP_LOG(TS_OFF, VLEVEL_M, "\r\n###### ========== MCPS-Confirm =============\r\n");
      APP_LOG(TS_OFF, VLEVEL_H, "###### U/L FRAME:%04d | PORT:%d | DR:%d | PWR:%d", params->UplinkCounter, //////////////////////////////////////////////////////////
              params->AppData.Port, params->Datarate, params->TxPower);

      APP_LOG(TS_OFF, VLEVEL_H, " | MSG TYPE:");
      if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG)
      {
        APP_LOG(TS_OFF, VLEVEL_H, "CONFIRMED [%s]\r\n", (params->AckReceived != 0) ? "ACK" : "NACK");
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_H, "UNCONFIRMED\r\n");
      }

      // ADDED LOGs ------------------------------------------------------------------

      APP_LOG(TS_OFF, VLEVEL_H, "###### PAYLOAD: ");
      if(AppData.BufferSize>0)
      {
		  for(int i=0;i<AppData.BufferSize;i++){
			  APP_LOG(TS_OFF, VLEVEL_H, "%x ", AppData.Buffer[i]);
		  }
      }

#ifdef CAYENNE_LPP

  if(ADMIN_PAYLOAD_HELLO)
  {
	  APP_LOG(TS_OFF, VLEVEL_H, " | SENDER TYPE: USER\r\n");
  }
    else
  {
    	APP_LOG(TS_OFF, VLEVEL_H, " | SENDER TYPE: CAYENNE LPP\r\n");
  }

#else  /* not CAYENNE_LPP */
  APP_LOG(TS_OFF, VLEVEL_H, " | SENDER TYPE: ELSE\r\n");
#endif /* CAYENNE_LPP */

  	  // END ADDED LOGs ------------------------------------------------------------------
    }
  }
  /* USER CODE END OnTxData_1 */
}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
  if (joinParams != NULL)
  {
    if (joinParams->Status == LORAMAC_HANDLER_SUCCESS)
    {
      UTIL_TIMER_Stop(&JoinLedTimer);
      BSP_LED_Off(LED_RED) ;

      APP_LOG(TS_OFF, VLEVEL_L, "\r\n> ...");
      APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOINED = ");
      if (joinParams->Mode == ACTIVATION_TYPE_ABP)
      {
        APP_LOG(TS_OFF, VLEVEL_L, "ABP !\r\n");
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_L, "OTAA !\r\n");
      }
    }
    else
    {
      APP_LOG(TS_OFF, VLEVEL_L, "\r\n> JOIN FAILED ...\r\n");
    }
    APP_LOG(0, 1, " \r\n");
  }
  /* USER CODE END OnJoinRequest_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */

  /* USER CODE END OnMacProcessNotify_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);

  /* USER CODE BEGIN OnMacProcessNotify_2 */

  /* USER CODE END OnMacProcessNotify_2 */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
