

#ifndef __CAYENNE_LPP_H__
#define __CAYENNE_LPP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


void CayenneLppInit(void);

void CayenneLppReset(void);

uint8_t CayenneLppGetSize(void);

uint8_t *CayenneLppGetBuffer(void);

uint8_t CayenneLppCopy(uint8_t *buffer);

uint8_t CayenneLppAddDigitalInput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddDigitalOutput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddAnalogInput(uint8_t channel, uint16_t value);

uint8_t CayenneLppAddAnalogOutput(uint8_t channel, uint16_t value);

uint8_t CayenneLppAddLuminosity(uint8_t channel, uint16_t lux);

uint8_t CayenneLppAddPresence(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddTemperature(uint8_t channel, int16_t celsius);

uint8_t CayenneLppAddRelativeHumidity(uint8_t channel, uint16_t rh);

uint8_t CayenneLppAddAccelerometer(uint8_t channel, int16_t x, int16_t y, int16_t z);

uint8_t CayenneLppAddBarometricPressure(uint8_t channel, uint16_t hpa);

uint8_t CayenneLppAddGyrometer(uint8_t channel, int16_t x, int16_t y, int16_t z);

uint8_t CayenneLppAddGps(uint8_t channel, int32_t latitude, int32_t longitude, int32_t meters);



#ifdef __cplusplus
}
#endif

#endif /* __CAYENNE_LPP_H__ */
