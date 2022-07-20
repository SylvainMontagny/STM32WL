/**
  ******************************************************************************
  * @file    kms_der_x962.h
  * @author  MCD Application Team
  * @brief   This file contains utilities for the manipulation of DER & X9.62 format
  *          of Key Management Services (KMS)
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_DER_X962_H
#define KMS_DER_X962_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_DER_X962 DER & ANS X9.62 utilities
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup KMS_DER_X962_Exported_Constants Exported Constants
  * @{
  */
#define KMS_X962_COMPRESSED_FORM             (3U)       /*!< X9.62 compressed form indication */
#define KMS_X962_UNCOMPRESSED_FORM           (4U)       /*!< X9.62 uncompressed form indication */

#define KMS_DER_OCTET_STRING                 (4U)       /*!< DER OCTET STRING indication */
/**
  * @}
  */
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @addtogroup KMS_DER_X962_Exported_Macros Exported Macros
  * @{
  */
#define KMS_IS_DER_OCTET_STRING(__ID__) \
  ((__ID__) == KMS_DER_OCTET_STRING)   /*!< Given parameter is first byte of an DER octet string */
#define KMS_IS_X962_COMPRESSED_FORM(__ID__) \
  ((__ID__) == KMS_X962_COMPRESSED_FORM)   /*!< Given parameter is first byte of an X9.62 compressed form of EC coordinates */
#define KMS_IS_X962_UNCOMPRESSED_FORM(__ID__) \
  ((__ID__) == KMS_X962_UNCOMPRESSED_FORM)   /*!< Given parameter is first byte of an X9.62 uncompressed form of EC coordinates */
/**
  * @}
  */
/* Exported functions prototypes ---------------------------------------------*/
/** @addtogroup KMS_DER_X962_Exported_Functions Exported Functions
  * @{
  */
CK_RV KMS_DerX962_OctetStringLength(uint8_t *ptr, uint32_t *pLen);
CK_RV KMS_DerX962_OctetStringDataOffset(uint8_t *ptr, uint32_t *pOff);
CK_RV KMS_DerX962_ExtractPublicKeyCoord(uint8_t *pDER, uint8_t *pX, uint8_t *pY, uint32_t ksize);
CK_RV KMS_DerX962_ConstructDERPublicKeyCoord(uint8_t *pX, uint8_t *pY, uint32_t ksize, uint8_t *ptr, uint32_t *dsize);
/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_DER_X962_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
