/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_sensors.h
  * @author  MCD Application Team
  * @brief   Header for sensors application
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORS_H__
#define __SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/**
  * Sensor data parameters
  */
typedef struct
{
  float pressure;         /*!< in mbar */
  float temperature;      /*!< in degC */
  float humidity;         /*!< in % */
  int32_t latitude;       /*!< latitude converted to binary */
  int32_t longitude ;     /*!< longitude converted to binary */
  int16_t altitudeGps;    /*!< in m */
  int16_t altitudeBar ;   /*!< in m * 10 */
  /**more may be added*/

} sensor_t;


#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1) && defined (X_NUCLEO_IKS01A2)
#define HTS221_0    0U
#define LPS22HB_0   1U
#endif /* SENSOR_ENABLED & X_NUCLEO_IKS01A2 */

int32_t EnvSensors_Init(void);

/**
  * @brief  Environmental sensor  read.
  * @param  sensor_data sensor data
  */
int32_t EnvSensors_Read(sensor_t *sensor_data);



#ifdef __cplusplus
}
#endif

#endif
