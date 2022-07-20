/**
  ******************************************************************************
  * @file    tkms.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for trusted Key Management
  *          Services (tKMS) module of the PKCS11 APIs when called from the
  *          secure enclave or without any enclave
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

/* Includes ------------------------------------------------------------------*/
#include "kms.h"
#if defined(KMS_ENABLED)
#include "tkms.h"
#include "kms_entry.h"

/** @addtogroup tKMS User interface to Key Management Services (tKMS)
  * @{
  */

/** @addtogroup KMS_TKMS_UTILS tKMS Utilities
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_TKMS_UTILS_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  This function is used to return the caller cluster number
  * @note   Declared as 'weak' it can be overloaded by caller code
  * @retval Caller cluster number
  */
#if defined (__ICCARM__)
__weak uint32_t tKMS_GetCluster(void)
#elif defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((weak)) uint32_t tKMS_GetCluster(void)
#endif
{
  return KMS_CLUST_SECX;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#endif /* KMS_ENABLED */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
