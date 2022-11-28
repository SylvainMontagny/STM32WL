
#include "app_lorawan.h"
#include "lora_app.h"
#include "sys_app.h"
#include "stm32_seq.h"


void MX_LoRaWAN_Init(void)
{
  SystemApp_Init();
  LoRaWAN_Init();
}

void MX_LoRaWAN_Process(void)
{
  UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
}

