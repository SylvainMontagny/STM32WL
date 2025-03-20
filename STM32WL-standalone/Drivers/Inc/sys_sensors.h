
#ifndef __SENSORS_H__
#define __SENSORS_H__


typedef struct
{
  int8_t header1;
  int8_t header2;
  int8_t header3;
  int8_t header4;
  int16_t sensor_status;
  int16_t temperature;      /* ! in °C * 100 */
  int16_t humidity;
  int16_t air_quality;
  int8_t end1;
  int8_t end2;
  int8_t end3;
  int8_t end4;
  int8_t end5;
  
} atim_payload_t;

typedef struct 
{
  /* Simple structure because watteco payload almost impossible to decode by hand */
  int64_t data[4];

} watteco_payload_t;


typedef struct 
{
  /* data */
  int8_t type;
  int8_t data_type1;
  int16_t temperature;  /* ! in °C * 100 */
  int8_t data_type2;
  int16_t courant;      /* ! in A * 100 */
  int8_t data_type3;
  int16_t voltage;      /* in mV */

} tct_payload_t;



typedef struct
{
  float stts751_temperature_float;      /*!< in degC */
  int8_t stts751_temperature_int8;

  float hts221_temperature_float;
  float hts221_humidity_float;
  int8_t hts221_temperature_int8;
  int8_t hts221_humidity_int8;

  int8_t stm32wl_temperature;

  int8_t temperature_simulated;
  int8_t humidity_simulated;
  int8_t setpoint;

  atim_payload_t atim_thaq;
  watteco_payload_t watteco_tempo;
  tct_payload_t tct_egreen;

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
