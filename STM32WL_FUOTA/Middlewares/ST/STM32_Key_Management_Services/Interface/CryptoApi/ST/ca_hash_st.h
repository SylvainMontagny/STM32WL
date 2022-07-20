/**
  ******************************************************************************
  * @file    ca_hash_st.h
  * @author  MCD Application Team
  * @brief   This file contains the HASH router includes and definitions of
  *          the Cryptographic API (CA) module to the ST Cryptographic library.
  * @note    This file shall never be included directly by application but
  *          through the main header ca_hash.h
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
#ifndef CA_HASH_ST_H
#define CA_HASH_ST_H

#if !defined(CA_HASH_H)
#error "This file shall never be included directly by application but through the main header ca_hash.h"
#endif /* CA_HASH_H */

/* Configuration defines -----------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "HASH/Common/hash_common.h"

/* Structures Remapping -------------------------------------------------------*/
typedef hashType_et CA_hashType_et;
typedef HashFlags_et CA_HashFlags_et;

/* Defines Remapping ----------------------------------------------------------*/
/* hashType_et values */
#define CA_E_SHA1   E_SHA1
#define CA_E_SHA256 E_SHA256

/* HashFlags_et values */
#define CA_E_HASH_DEFAULT                     E_HASH_DEFAULT
#define CA_E_HASH_DONT_PERFORM_KEY_SCHEDULE   E_HASH_DONT_PERFORM_KEY_SCHEDULE
#define CA_E_HASH_OPERATION_COMPLETED         E_HASH_OPERATION_COMPLETED
#define CA_E_HASH_NO_MORE_APPEND_ALLOWED      E_HASH_NO_MORE_APPEND_ALLOWED

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> HASH SHA1 */
#if defined(CA_ROUTE_HASH_SHA1) && ((CA_ROUTE_HASH_SHA1 & CA_ROUTE_MASK) == CA_ROUTE_ST)
/* Defines Remapping ----------------------------------------------------------*/
#ifndef INCLUDE_SHA1
#define INCLUDE_SHA1
#endif /* INCLUDE_SHA1 */

#include "HASH/SHA1/sha1.h"

/* Structures Remapping -------------------------------------------------------*/
typedef SHA1ctx_stt CA_SHA1ctx_stt;

/* API Remapping --------------------------------------------------------------*/
#define CA_SHA1_Init     SHA1_Init
#define CA_SHA1_Append   SHA1_Append
#define CA_SHA1_Finish   SHA1_Finish

#endif /* CA_ROUTE_HASH_SHA1 == CA_ROUTE_ST */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< HASH SHA1 */

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> HASH SHA256 */
#if defined(CA_ROUTE_HASH_SHA256) && ((CA_ROUTE_HASH_SHA256 & CA_ROUTE_MASK) == CA_ROUTE_ST)
/* Defines Remapping ----------------------------------------------------------*/
#ifndef INCLUDE_SHA256
#define INCLUDE_SHA256
#endif /* INCLUDE_SHA256 */

#include "HASH/SHA256/sha256.h"

/* Structures Remapping -------------------------------------------------------*/
typedef SHA256ctx_stt CA_SHA256ctx_stt;

/* API Remapping --------------------------------------------------------------*/
#define CA_SHA256_Init     SHA256_Init
#define CA_SHA256_Append   SHA256_Append
#define CA_SHA256_Finish   SHA256_Finish

#endif /* CA_ROUTE_HASH_SHA256 == CA_ROUTE_ST */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< HASH SHA256 */


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CA_HASH_ST_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

