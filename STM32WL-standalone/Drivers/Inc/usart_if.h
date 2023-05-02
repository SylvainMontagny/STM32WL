

#include "stm32_adv_trace.h"
#include "usart.h"
#include "dma.h"


#ifndef __USART_IF_H__
#define __USART_IF_H__

#ifdef __cplusplus
extern "C" {
#endif



/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Init the UART and associated DMA.
  * @param  cb TxCpltCallback
  * @return @ref UTIL_ADV_TRACE_Status_t
  */
UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *));

/**
  * @brief  init receiver of vcom
  * @param  RxCb callback when Rx char is received
  * @return @ref UTIL_ADV_TRACE_Status_t
  */
UTIL_ADV_TRACE_Status_t vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error));

/**
  * @brief  DeInit the UART and associated DMA.
  * @return @ref UTIL_ADV_TRACE_Status_t
  */
UTIL_ADV_TRACE_Status_t vcom_DeInit(void);

/**
  * @brief  send buffer \p p_data of size \p size to vcom in polling mode
  * @param  p_data data to be sent
  * @param  size of buffer p_data to be sent
  */
void vcom_Trace(uint8_t *p_data, uint16_t size);

/**
  * @brief  send buffer \p p_data of size \p size to vcom using DMA
  * @param  p_data data to be sent
  * @param  size of buffer p_data to be sent
  * @return @ref UTIL_ADV_TRACE_Status_t
  */
UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size);

/**
  * @brief  last byte has been sent on the uart line
  */
void vcom_IRQHandler(void);

/**
  * @brief  last byte has been sent from memory to uart data register
  */
void vcom_DMA_TX_IRQHandler(void);

/**
  * @brief  Resume the UART and associated DMA (used by LPM)
  */
void vcom_Resume(void);



#ifdef __cplusplus
}
#endif

#endif
