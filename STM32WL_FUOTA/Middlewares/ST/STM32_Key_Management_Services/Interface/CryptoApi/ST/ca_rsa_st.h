/**
  ******************************************************************************
  * @file    ca_rsa_st.h
  * @author  MCD Application Team
  * @brief   This file contains the RSA router includes and definitions of
  *          the Cryptographic API (CA) module to the ST Cryptographic library.
  * @note    This file shall never be included directly by application but
  *          through the main header ca_rsa.h
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
#ifndef CA_RSA_ST_H
#define CA_RSA_ST_H

#if !defined(CA_RSA_H)
#error "This file shall never be included directly by application but through the main header ca_rsa.h"
#endif /* CA_RSA_H */

/* Configuration defines -----------------------------------------------------*/
#ifndef INCLUDE_RSA
#define INCLUDE_RSA
#endif /* INCLUDE_RSA */

/* Includes ------------------------------------------------------------------*/
#include "RSA/rsa.h"

/* Structures Remapping -------------------------------------------------------*/

/* Defines Remapping ----------------------------------------------------------*/

/* API Remapping --------------------------------------------------------------*/

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RSA */
#if defined(CA_ROUTE_RSA) && ((CA_ROUTE_RSA & CA_ROUTE_MASK) == CA_ROUTE_ST)
/* Includes ------------------------------------------------------------------*/

/* Defines ------------------------------------------------------------------*/
#if ((CA_ROUTE_RSA & CA_ROUTE_RSA_CFG_2048BITS_ENABLE) == CA_ROUTE_RSA_CFG_2048BITS_ENABLE)
#define CA_RSA_REQUIRED_WORKING_BUFFER    4096
#else /* (CA_ROUTE_RSA & CA_ROUTE_RSA_CFG_2048BITS_ENABLE) == CA_ROUTE_RSA_CFG_2048BITS_ENABLE */
#define CA_RSA_REQUIRED_WORKING_BUFFER    2048
#endif /* (CA_ROUTE_RSA & CA_ROUTE_RSA_CFG_2048BITS_ENABLE) == CA_ROUTE_RSA_CFG_2048BITS_ENABLE */

/* Defines Remapping ----------------------------------------------------------*/

/* Structures Remapping -------------------------------------------------------*/
typedef RSApubKey_stt   CA_RSApubKey_stt;
typedef RSAprivKey_stt  CA_RSAprivKey_stt;
typedef RSAinOut_stt    CA_RSAinOut_stt;

/* API Remapping --------------------------------------------------------------*/
#define CA_RSA_PKCS1v15_Sign    RSA_PKCS1v15_Sign
#define CA_RSA_PKCS1v15_Verify  RSA_PKCS1v15_Verify

#endif /* CA_ROUTE_RSA == CA_ROUTE_ST */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RSA */



#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CA_RSA_ST_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

