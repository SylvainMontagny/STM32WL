
#include "stm32_mem.h"
#include "CayenneLpp.h"

#define CAYENNE_LPP_MAXBUFFER_SIZE                  242
#define LPP_DIGITAL_INPUT       0       /* 1 byte */
#define LPP_DIGITAL_OUTPUT      1       /* 1 byte */
#define LPP_ANALOG_INPUT        2       /* 2 bytes, 0.01 signed */
#define LPP_ANALOG_OUTPUT       3       /* 2 bytes, 0.01 signed */
#define LPP_LUMINOSITY          101     /* 2 bytes, 1 lux unsigned */
#define LPP_PRESENCE            102     /* 1 byte, 1 */
#define LPP_TEMPERATURE         103     /* 2 bytes, 0.1 Celsius degrees signed */
#define LPP_RELATIVE_HUMIDITY   104     /* 1 byte, 0.5% unsigned */
#define LPP_ACCELEROMETER       113     /* 2 bytes per axis, 0.001G */
#define LPP_BAROMETRIC_PRESSURE 115     /* 2 bytes 0.1 hPa Unsigned */
#define LPP_GYROMETER           134     /* 2 bytes per axis, 0.01 degrees/s */
#define LPP_GPS                 136     /* 3 byte lon/lat 0.0001 degrees, 3 bytes alt 0.01m */

/* Data ID + Data Type + Data Size */
#define LPP_DIGITAL_INPUT_SIZE       3
#define LPP_DIGITAL_OUTPUT_SIZE      3
#define LPP_ANALOG_INPUT_SIZE        4
#define LPP_ANALOG_OUTPUT_SIZE       4
#define LPP_LUMINOSITY_SIZE          4
#define LPP_PRESENCE_SIZE            3
#define LPP_TEMPERATURE_SIZE         4
#define LPP_RELATIVE_HUMIDITY_SIZE   3
#define LPP_ACCELEROMETER_SIZE       8
#define LPP_BAROMETRIC_PRESSURE_SIZE 4
#define LPP_GYROMETER_SIZE           8
#define LPP_GPS_SIZE                 11


static uint8_t CayenneLppBuffer[CAYENNE_LPP_MAXBUFFER_SIZE];
static uint8_t CayenneLppCursor = 0;

void CayenneLppInit(void)
{
  CayenneLppCursor = 0;
}

void CayenneLppReset(void)
{
  CayenneLppCursor = 0;
}

uint8_t CayenneLppGetSize(void)
{
  return CayenneLppCursor;
}

uint8_t *CayenneLppGetBuffer(void)
{
  return CayenneLppBuffer;
}

uint8_t CayenneLppCopy(uint8_t *dst)
{
  UTIL_MEM_cpy_8(dst, CayenneLppBuffer, CayenneLppCursor);
  return CayenneLppCursor;
}

uint8_t CayenneLppAddDigitalInput(uint8_t channel, uint8_t value)
{

  if ((CayenneLppCursor + LPP_DIGITAL_INPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_INPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddDigitalOutput(uint8_t channel, uint8_t value)
{

  if ((CayenneLppCursor + LPP_DIGITAL_OUTPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_OUTPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value;

  return CayenneLppCursor;
}

uint8_t CayenneLppAddAnalogInput(uint8_t channel, uint16_t value)
{
  if ((CayenneLppCursor + LPP_ANALOG_INPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }

  value *= 100;
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_INPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddAnalogOutput(uint8_t channel, uint16_t value)
{
  if ((CayenneLppCursor + LPP_ANALOG_OUTPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  value *= 100;
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_OUTPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddLuminosity(uint8_t channel, uint16_t lux)
{
  if ((CayenneLppCursor + LPP_LUMINOSITY_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_LUMINOSITY;
  CayenneLppBuffer[CayenneLppCursor++] = lux >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lux;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddPresence(uint8_t channel, uint8_t value)
{
  if ((CayenneLppCursor + LPP_PRESENCE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_PRESENCE;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddTemperature(uint8_t channel, int16_t celsius)
{
  if ((CayenneLppCursor + LPP_TEMPERATURE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t val = celsius * 10;
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_TEMPERATURE;
  CayenneLppBuffer[CayenneLppCursor++] = val >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = val;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddRelativeHumidity(uint8_t channel, uint16_t rh)
{
  if ((CayenneLppCursor + LPP_RELATIVE_HUMIDITY_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_RELATIVE_HUMIDITY;
  CayenneLppBuffer[CayenneLppCursor++] = rh * 2;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddAccelerometer(uint8_t channel, int16_t x, int16_t y, int16_t z)
{
  if ((CayenneLppCursor + LPP_ACCELEROMETER_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t vx = x * 1000;
  int16_t vy = y * 1000;
  int16_t vz = z * 1000;

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ACCELEROMETER;
  CayenneLppBuffer[CayenneLppCursor++] = vx >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vx;
  CayenneLppBuffer[CayenneLppCursor++] = vy >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vy;
  CayenneLppBuffer[CayenneLppCursor++] = vz >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vz;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddBarometricPressure(uint8_t channel, uint16_t hpa)
{
  if ((CayenneLppCursor + LPP_BAROMETRIC_PRESSURE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  hpa *= 10;

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_BAROMETRIC_PRESSURE;
  CayenneLppBuffer[CayenneLppCursor++] = hpa >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = hpa;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddGyrometer(uint8_t channel, int16_t x, int16_t y, int16_t z)
{
  if ((CayenneLppCursor + LPP_GYROMETER_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t vx = x * 100;
  int16_t vy = y * 100;
  int16_t vz = z * 100;

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_GYROMETER;
  CayenneLppBuffer[CayenneLppCursor++] = vx >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vx;
  CayenneLppBuffer[CayenneLppCursor++] = vy >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vy;
  CayenneLppBuffer[CayenneLppCursor++] = vz >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vz;
  return CayenneLppCursor;
}

uint8_t CayenneLppAddGps(uint8_t channel, int32_t latitude, int32_t longitude, int32_t meters)
{
  if ((CayenneLppCursor + LPP_GPS_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int32_t lat = latitude * 10000;
  int32_t lon = longitude * 10000;
  int32_t alt = meters * 100;

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_GPS;

  CayenneLppBuffer[CayenneLppCursor++] = lat >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = lat >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lat;
  CayenneLppBuffer[CayenneLppCursor++] = lon >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = lon >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lon;
  CayenneLppBuffer[CayenneLppCursor++] = alt >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = alt >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = alt;
  return CayenneLppCursor;
}


