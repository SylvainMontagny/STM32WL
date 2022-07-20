/**
  ******************************************************************************
  * @file    ca_rng.h
  * @author  MCD Application Team
  * @brief   This file contains the RNG router includes and definitions of
  *          the Cryptographic API (CA) module.
  * @note    This file shall never be included directly by application but
  *          through the main header ca.h
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
#ifndef CA_RNG_H
#define CA_RNG_H

#if !defined(CA_H)
#error "This file shall never be included directly by application but through the main header ca.h"
#endif /* CA_H */

/* Configuration defines -----------------------------------------------------*/



/* Includes ------------------------------------------------------------------*/

#if defined(CA_ST_CRYPTOLIB_SUPP)
#include "ST/ca_rng_st.h"
#endif /* CA_ST_CRYPTOLIB_SUPP */

#if defined(CA_MBED_CRYPTOLIB_SUPP)
#include "MBED/ca_rng_mbed.h"
#endif /* CA_MBED_CRYPTOLIB_SUPP */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CA_RNG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

