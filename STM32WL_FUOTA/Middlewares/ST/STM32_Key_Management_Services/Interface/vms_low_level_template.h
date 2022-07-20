/**
  ******************************************************************************
  * @file    vms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module VM Low Level access to RAM storage
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

#ifndef VMS_LOW_LEVEL_H
#define VMS_LOW_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Includes */
/* USER CODE END VMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_VMLL VM Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Types */
/* USER CODE END VMS_LOW_LEVEL_Exported_Types */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Constants */
/* USER CODE END VMS_LOW_LEVEL_Exported_Constants */

/** @addtogroup KMS_VMLL_Exported_Constants Exported Constants
  * @{
  */

#define VMS_LL_ERASED           0xFFFFFFFFU    /*!< Value used to notifies data has been erased */

/**
  * @brief VM writing granularity size
  * @note  Not physical page size, but writing granularity size -> 8 bytes
  */
#define VMS_LL_PAGE_SIZE    8


/**
  * @}
  */


/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Function_Prototypes */
/* USER CODE END VMS_LOW_LEVEL_Exported_Function_Prototypes */

/** @addtogroup KMS_VMLL_Exported_Functions Exported Functions
  * @{
  */

/* Exported in line functions ------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_In_Line_Function */
/* USER CODE END VMS_LOW_LEVEL_Exported_In_Line_Function */

/**
  * @brief   Returns the base address of the KMS VM data storage
  * @retval  pointer to the base of the KMS VM data storage
  */
/* Template version */
static inline uint32_t VMS_LL_GetDataStorageAddress(void)
{
  /* USER CODE BEGIN VMS_LL_GetDataStorageAddress */
  return 0UL;
  /* USER CODE END VMS_LL_GetDataStorageAddress */
}

/**
  * @brief   Returns the size of the KMS VM data storage
  * @retval  KMS VM data storage size
  */
/* Template version */
static inline size_t VMS_LL_GetDataStorageSize(void)
{
  /* USER CODE BEGIN VMS_LL_GetDataStorageSize */
  return 0UL;
  /* USER CODE END VMS_LL_GetDataStorageSize */
}

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

#endif /* VMS_LOW_LEVEL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
