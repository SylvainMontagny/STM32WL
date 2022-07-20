/**
  ******************************************************************************
  * @file    kms_dyn_obj.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module dynamic object management functionalities.
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
#ifndef KMS_DYN_OBJ_H
#define KMS_DYN_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_DYNOBJ Objects services
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

CK_RV  KMS_CreateObject(CK_SESSION_HANDLE hSession,
                        CK_ATTRIBUTE_PTR pTemplate,
                        CK_ULONG ulCount,
                        CK_OBJECT_HANDLE_PTR phObject);

CK_RV  KMS_DestroyObject(CK_SESSION_HANDLE hSession,
                         CK_OBJECT_HANDLE hObject);

CK_RV KMS_GetAttributeValue(CK_SESSION_HANDLE hSession,  CK_OBJECT_HANDLE  hObject,
                            CK_ATTRIBUTE_PTR  pTemplate, CK_ULONG          ulCount);

CK_RV  KMS_FindObjectsInit(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);

CK_RV KMS_FindObjects(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE_PTR phObject,
                      CK_ULONG ulMaxObjectCount,  CK_ULONG_PTR pulObjectCount);

CK_RV KMS_FindObjectsFinal(CK_SESSION_HANDLE hSession);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_DYN_OBJ_H */
