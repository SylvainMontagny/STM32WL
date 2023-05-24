
#include "stdint.h"
#include "sys_conf.h"
#include "sys_app.h"
#include "sys_sensors.h"
#include "stts751_reg.h"
#include "i2c.h"

int16_t stts751_raw_temperature;
static stts751_id_t whoamI;
static uint8_t tx_buffer[100];


stmdev_ctx_t dev_ctx; // Not used but to comply with ST library


int32_t  EnvSensors_Read(sensor_t *sensor_data)
{
#if SENSOR_ENABLED == 1
	memset(&stts751_raw_temperature, 0, sizeof(int16_t));
	stts751_temperature_raw_get(&dev_ctx, &stts751_raw_temperature);
	sensor_data->stts751_temperature = stts751_from_lsb_to_celsius( stts751_raw_temperature );
	//sprintf((char *)tx_buffer, "Temperature [degC]:%3.2f\r\n",  temperature_degC);
	//APP_LOG(0, 1,tx_buffer);
#endif
	return 0;

}

int32_t  EnvSensors_Init(void)
{
#if SENSOR_ENABLED == 1

	stts751_device_id_get(&dev_ctx, &whoamI);
	if ( (whoamI.product_id != STTS751_ID_1xxxx) || (whoamI.manufacturer_id != STTS751_ID_MAN) || (whoamI.revision_id != STTS751_REV) ){
		APP_LOG_COLOR(RED);
		APP_LOG(0, 1, "\n\r> Temperature sensor STTS751 not found\r\n");
	}
	else{
		APP_LOG_COLOR(GREEN);
		APP_LOG(0, 1, "\r\n> Temperature sensor STTS751 ready\r\n");
	}
	APP_LOG_COLOR(RESET_COLOR);
	stts751_temp_data_rate_set(&dev_ctx, STTS751_TEMP_ODR_1Hz);
	stts751_resolution_set(&dev_ctx, STTS751_11bit);

#endif
	return 0;

}


/**
  * @brief  Read generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to read
  * @param  data  pointer to buffer that store the data read(ptr)
  * @param  len   number of consecutive register to read
  * @retval          interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t stts751_read_reg(stmdev_ctx_t *ctx, uint8_t reg,
                                uint8_t *data,
                                uint16_t len)
{


	 HAL_I2C_Mem_Read(&hi2c2, STTS751_1xxxx_ADD_7K5, reg, I2C_MEMADD_SIZE_8BIT, data, len, 1000);

  return 0;
}

/**
  * @brief  Write generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to write
  * @param  data  pointer to data to write in register reg(ptr)
  * @param  len   number of consecutive register to write
  * @retval          interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t stts751_write_reg(stmdev_ctx_t *ctx, uint8_t reg,
                                 uint8_t *data,
                                 uint16_t len)
{


  HAL_I2C_Mem_Write(&hi2c2, STTS751_1xxxx_ADD_7K5, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) data, len, 1000);

  return 0;
}





