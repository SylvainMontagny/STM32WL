/**
  ******************************************************************************
  * @file    kms_key_mgt.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module key handling functionalities.
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
#ifndef KMS_KEY_MGT_H
#define KMS_KEY_MGT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_KEY Key handling services
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/**
  * @}
  */


/** @addtogroup KMS_API KMS APIs (PKCS#11 Standard Compliant)
  * @{
  */

CK_RV KMS_DeriveKey(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                    CK_OBJECT_HANDLE hBaseKey, CK_ATTRIBUTE_PTR  pTemplate,
                    CK_ULONG  ulAttributeCount, CK_OBJECT_HANDLE_PTR  phKey);
CK_RV KMS_GenerateKeyPair(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism,
                          CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount,
                          CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount,
                          CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey);
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_KEY_MGT_H */


