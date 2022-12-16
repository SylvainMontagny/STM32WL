
#ifndef __UTILITIES_DEF_H__
#define __UTILITIES_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
  * Supported requester to the MCU Low Power Manager - can be increased up  to 32
  * It lists a bit mapping of all user of the Low Power Manager
  */
typedef enum
{
  /* USER CODE BEGIN CFG_LPM_Id_t_0 */

  /* USER CODE END CFG_LPM_Id_t_0 */
  CFG_LPM_APPLI_Id,
  CFG_LPM_UART_TX_Id,
  /* USER CODE BEGIN CFG_LPM_Id_t */

  /* USER CODE END CFG_LPM_Id_t */
} CFG_LPM_Id_t;

/*---------------------------------------------------------------------------*/
/*                             sequencer definitions                         */
/*---------------------------------------------------------------------------*/

/**
  * This is the list of priority required by the application
  * Each Id shall be in the range 0..31
  */
typedef enum
{
  CFG_SEQ_Prio_0,

  CFG_SEQ_Prio_NBR,
} CFG_SEQ_Prio_Id_t;

/**
  * This is the list of task id required by the application
  * Each Id shall be in the range 0..31
  */
typedef enum
{
  CFG_SEQ_Task_LmHandlerProcess,
  CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent,

  CFG_SEQ_Task_NBR
} CFG_SEQ_Task_Id_t;



#ifdef __cplusplus
}
#endif

#endif /* __UTILITIES_DEF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
