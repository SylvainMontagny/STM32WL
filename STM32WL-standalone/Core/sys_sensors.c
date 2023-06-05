#include "stdio.h"
#include "stdint.h"
#include "sys_conf.h"
#include "sys_app.h"
#include "sys_sensors.h"

#include "stts751_reg.h"
#include "hts221_reg.h"

#include "i2c.h"

static stts751_id_t stts751_whoamI;
static int16_t stts751_raw_temperature;

static uint8_t hts221_whoamI;
static int16_t hts221_raw_humidity;
static int16_t hts221_raw_temperature;
static hts221_lin_t lin_hum;
static hts221_lin_t lin_temp;

static uint8_t tx_buffer[100];	// Only useful if we need to log the sensor value


stmdev_ctx_t dev_ctx; // Not used but to comply with ST library


int32_t  EnvSensors_Read(sensor_t *sensor_data)
{
#if SENSOR_ENABLED == 1

	/***** STTS751 - Temperature  ****/
	memset(&stts751_raw_temperature, 0, sizeof(int16_t));
	stts751_temperature_raw_get(&dev_ctx, &stts751_raw_temperature);
	sensor_data->stts751_temperature_int8 = (int8_t)(stts751_raw_temperature >> 8);
	sensor_data->stts751_temperature_float = stts751_from_lsb_to_celsius( stts751_raw_temperature );
	//sprintf((char *)tx_buffer, "Temperature [degC]:%3.2f\r\n",  temperature_degC);
	//APP_LOG(0, 1,tx_buffer);

	/***** HTS221 - Temperature - Humidity  ****/
    hts221_reg_t reg;
    hts221_status_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.h_da) {
      /* Read humidity data */
      memset(&hts221_raw_humidity, 0x00, sizeof(int16_t));
      hts221_humidity_raw_get(&dev_ctx, &hts221_raw_humidity);
      sensor_data->hts221_humidity_float = hts221_linear_interpolation(&lin_hum, hts221_raw_humidity);

      if (sensor_data->hts221_humidity_float < 0) {
    	  sensor_data->hts221_humidity_float = 0;
      }

      if (sensor_data->hts221_humidity_float > 100) {
    	  sensor_data->hts221_humidity_float = 100;
      }
      sensor_data->hts221_humidity_int8  =  (int8_t)sensor_data->hts221_humidity_float;

      //sprintf((char *)tx_buffer, "Humidity [%%]:%3.2f\r\n", sensor_data->hts221_humidity_float);
      //APP_LOG(0, 1,tx_buffer);
    }

    if (reg.status_reg.t_da) {
      /* Read temperature data */
      memset(&hts221_raw_temperature, 0x00, sizeof(int16_t));
      hts221_temperature_raw_get(&dev_ctx, &hts221_raw_temperature);
      sensor_data->hts221_temperature_float =  hts221_linear_interpolation(&lin_temp, hts221_raw_temperature);
      //sprintf((char *)tx_buffer, "Temperature [degC]:%6.2f\r\n", sensor_data->hts221_temperature_float );
      //APP_LOG(0, 1,tx_buffer);
    }
#endif
	return 0;

}

int32_t  EnvSensors_Init(void)
{
#if SENSOR_ENABLED == 1

	/***** STTS751 - Temperature  ****/
	stts751_device_id_get(&dev_ctx, &stts751_whoamI);
	if ( (stts751_whoamI.product_id != STTS751_ID_1xxxx) || (stts751_whoamI.manufacturer_id != STTS751_ID_MAN) || (stts751_whoamI.revision_id != STTS751_REV) ){
		APP_LOG_COLOR(RED);
		APP_LOG(0, 1, "\n\r> Temperature sensor STTS751 not found\r\n");
	}
	else{
		APP_LOG_COLOR(GREEN);
		APP_LOG(0, 1, "\r\n> Temperature sensor STTS751 ready\r\n");
		APP_LOG_COLOR(RESET_COLOR);
		stts751_temp_data_rate_set(&dev_ctx, STTS751_TEMP_ODR_1Hz);
		stts751_resolution_set(&dev_ctx, STTS751_11bit);
	}


	/***** HTS221 - Temperature - Humidity  ****/
	hts221_whoamI = 0;
	hts221_device_id_get(&dev_ctx, &hts221_whoamI);

	if ( hts221_whoamI != HTS221_ID ){
		APP_LOG_COLOR(RED);
		APP_LOG(0, 1, "> Temperature - Humidity sensor HTS221 not found\r\n");
	}
	else{
		APP_LOG_COLOR(GREEN);
		APP_LOG(0, 1, "> Temperature - Humidity sensor HTS221 ready\r\n");
		APP_LOG_COLOR(RESET_COLOR);
		/* Read humidity calibration coefficient */
		hts221_hum_adc_point_0_get(&dev_ctx, &lin_hum.x0);
		hts221_hum_rh_point_0_get(&dev_ctx, &lin_hum.y0);
		hts221_hum_adc_point_1_get(&dev_ctx, &lin_hum.x1);
		hts221_hum_rh_point_1_get(&dev_ctx, &lin_hum.y1);
		/* Read temperature calibration coefficient */
		hts221_temp_adc_point_0_get(&dev_ctx, &lin_temp.x0);
		hts221_temp_deg_point_0_get(&dev_ctx, &lin_temp.y0);
		hts221_temp_adc_point_1_get(&dev_ctx, &lin_temp.x1);
		hts221_temp_deg_point_1_get(&dev_ctx, &lin_temp.y1);
		/* Enable Block Data Update */
		hts221_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
		/* Set Output Data Rate */
		hts221_data_rate_set(&dev_ctx, HTS221_ODR_1Hz);
		/* Device power on */
		hts221_power_on_set(&dev_ctx, PROPERTY_ENABLE);
	}

#endif
	return 0;

}


	/***** STTS751 - Temperature  ****/
int32_t stts751_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	HAL_I2C_Mem_Read(&hi2c2, STTS751_1xxxx_ADD_7K5, reg, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
	return 0;
}

int32_t stts751_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	HAL_I2C_Mem_Write(&hi2c2, STTS751_1xxxx_ADD_7K5, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) data, len, 1000);
	return 0;
}

	/***** HTS221 - Temperature - Humidity  ****/

float hts221_linear_interpolation(hts221_lin_t *lin, int16_t x)
{
  return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) -
                                     (lin->x0 * lin->y1)))
         / (lin->x1 - lin->x0);
}


int32_t hts221_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	reg |= 0x80;
	HAL_I2C_Mem_Read(&hi2c2, HTS221_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
	return 0;
}

int32_t hts221_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len)
{
	reg |= 0x80;
	HAL_I2C_Mem_Write(&hi2c2, HTS221_I2C_ADDRESS, reg, 	I2C_MEMADD_SIZE_8BIT, (uint8_t*) data, len, 1000);
  return 0;
}
