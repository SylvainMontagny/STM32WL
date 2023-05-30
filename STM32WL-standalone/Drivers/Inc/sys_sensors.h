
#ifndef __SENSORS_H__
#define __SENSORS_H__

typedef struct
{
  float stts751_temperature_float;      /*!< in degC */
  int8_t stts751_temperature_int8;

  float hts221_temperature_float;
  float hts221_humidity_float;

  int8_t stm32wl_temperature;

  int8_t temperature_simulated;
  int8_t humidity_simulated;

} sensor_t;


typedef struct {
  float x0;
  float y0;
  float x1;
  float y1;
} hts221_lin_t;

int32_t EnvSensors_Init(void);
int32_t EnvSensors_Read(sensor_t *sensor_data);

float hts221_linear_interpolation(hts221_lin_t *lin, int16_t x);


#endif
