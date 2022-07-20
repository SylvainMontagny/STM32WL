/**
  ******************************************************************************
  * @file    kms_ext_token.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for the External Token services
  *          to Key Management Services.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_EXT_TOKEN_H
#define KMS_EXT_TOKEN_H


#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

#ifdef KMS_EXT_TOKEN_ENABLED

/** @addtogroup KMS_EXTTOKEN External Token services
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_EXTTOKEN_Exported_Functions Exported Functions
  * @{
  */
/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */
CK_RV KMS_EXT_TOKEN_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);

/**
  * @}
  */

#endif /* KMS_EXT_TOKEN_ENABLED */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_EXT_TOKEN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
