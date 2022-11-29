

#ifndef __LORA_INFO_H__
#define __LORA_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


/*!
 * To give info to the application about LoraWAN capability
 * it can depend how it has been compiled (e.g. compiled regions ...)
 * Params should be better uint32_t foe easier alignment with info_table concept
 */
typedef struct
{
  uint32_t ActivationMode;  /*!< 1: ABP, 2 : OTAA, 3: ABP & OTAA   */
  uint32_t Region;   /*!< Combination of regions compiled on MW  */
  uint32_t ClassB;   /*!< 0: not compiled in Mw, 1 : compiled in MW  */
  uint32_t Kms;      /*!< 0: not compiled in Mw, 1 : compiled in MW  */
} LoraInfo_t;


void LoraInfo_Init(void);

/**
  * @brief returns the pointer to the LoraInfo capabilities table
  * @retval LoraInfo pointer
  */
LoraInfo_t *LoraInfo_GetPtr(void);



#ifdef __cplusplus
}
#endif

#endif
