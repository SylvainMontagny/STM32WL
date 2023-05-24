
#ifndef __SENSORS_H__
#define __SENSORS_H__

typedef struct
{
  float pressure;         /*!< in mbar */
  float stts751_temperature;      /*!< in degC */
  float humidity;         /*!< in % */

} sensor_t;


int32_t EnvSensors_Init(void);

int32_t EnvSensors_Read(sensor_t *sensor_data);


#endif
