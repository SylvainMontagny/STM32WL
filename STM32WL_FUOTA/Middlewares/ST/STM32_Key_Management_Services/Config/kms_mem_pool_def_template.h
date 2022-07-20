/**
  ******************************************************************************
  * @file    kms_mem_pool_def.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          memory pool definition
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

/* Recursive inclusion autorised ---------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * POOL DESCRIPTION
 */

/*
 * In the below example, the described memory is made of 3 pools
 * - Pool #1 is a 256 Bytes pool
 * - Pool #2 is a 256 Bytes pool
 * - Pool #3 is a 512 Bytes pool
 */
/*
KMS_MEM_DECLARE_POOL_START()
KMS_MEM_DECLARE_POOL_ENTRY(1,256)
KMS_MEM_DECLARE_POOL_ENTRY(2,256)
KMS_MEM_DECLARE_POOL_ENTRY(3,512)
KMS_MEM_DECLARE_POOL_END()
*/

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

