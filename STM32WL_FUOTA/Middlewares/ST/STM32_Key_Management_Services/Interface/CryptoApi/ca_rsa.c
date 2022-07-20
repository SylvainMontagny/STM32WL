/**
  ******************************************************************************
  * @file    ca_rsa.c
  * @author  MCD Application Team
  * @brief   This file constitutes the Cryptographic API (CA) module RSA sources
  *          as its inclusion allows based on the configuration to build every
  *          needed other RSA related c files contents
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

/* CA sources are built by building ca_core.c giving it the proper ca_config.h */
/* This file can not be build alone                                            */
#if defined(CA_CORE_C)

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CA_RSA_C
#define CA_RSA_C

/* Includes ------------------------------------------------------------------*/

#if defined(CA_ST_CRYPTOLIB_SUPP)
/* No sources to include */
#endif /* CA_ST_CRYPTOLIB_SUPP */

#if defined(CA_MBED_CRYPTOLIB_SUPP)
#include "MBED/ca_rsa_mbed.c"
#endif /* CA_MBED_CRYPTOLIB_SUPP */

#if defined(CA_HAL_CRYPTOLIB_SUPP)
#include "HAL/ca_rsa_hal.c"
#endif /* CA_HAL_CRYPTOLIB_SUPP */

#endif /* CA_RSA_C */
#endif /* CA_CORE_C */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

