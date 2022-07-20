/**
  ******************************************************************************
  * @file    kms_mem_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          memory management Low Level function to custom memory allocator
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

#include "kms.h"
#include "kms_mem.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_MEM_LL Memory management Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/**
  * @brief  Initialize memory management
  */
void KMS_MemInit(void)
{
  /* Put your own implementation here */
}

/**
  * @brief  Allocate memory
  * @param  session Session requesting the memory
  * @param  size Size of the memory to allocate
  * @retval Allocated pointer if successful to allocate, NULL_PTR if failed
  */
CK_VOID_PTR KMS_Alloc(CK_SESSION_HANDLE session, size_t size)
{
  /* Put your own implementation here */
}

/**
  * @brief  Free memory
  * @param  session Session freeing the memory
  * @param  ptr     Pointer to the memory to free
  * @retval None
  */
void KMS_Free(CK_SESSION_HANDLE session, CK_VOID_PTR ptr)
{
  /* Put your own implementation here */
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
