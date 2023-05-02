

#ifndef APP_SEND_RAW_LORA_H_
#define APP_SEND_RAW_LORA_H_

//#include "core_cm4.h"
#include <stdint.h>

#define TEST_LORA                     1
#define EMISSION_POWER                -9
#define CR4o5                         1
#define DEFAULT_PAYLOAD_LEN           5
#define DEFAULT_FSK_DEVIATION         25000
#define DEFAULT_LDR_OPT               2
#define DEFAULT_GAUSS_BT              3
#define LORA_PREAMBLE_LENGTH          8
#define TX_TIMEOUT_VALUE              3000


void LoRa_OnTxDone(void);
void LoRa_OnRxDone(void);
void LoRa_OnTxTimeout(void);
void LoRa_OnRxTimeout(void);
void LoRa_OnRxError(void);
void LoRa_Send(void);


typedef struct
{
  uint32_t modulation;         // 0: FSK  -  1: Lora  -  2:BPSK
  uint32_t freq;               // in Hz
  int32_t power;               // [-9 :22] dBm
  uint32_t bandwidth;          // Lora [ 0:7.8125  -  1:15.625  -  2:31.25  - 3:62.5 -  4:125  -  5:250  -  6:500] kHz

  uint32_t loraSf_datarate;    // [ 7 : 12]
  uint32_t codingRate;         // Lora Only [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
  uint32_t lna;                // 0:off  -  1:On
  uint32_t paBoost;            // 0:off  -  1:On
  uint32_t payloadLen;         // [1:256]
  uint32_t fskDev;             // FSK only

  uint32_t lowDrOpt;           // Lora Only 0: off, 1:On, 2: Auto (1 when SF11 or SF12, 0 otherwise)
  uint32_t BTproduct;          // FSK only
} testParameter_t;

typedef enum
{
  CFG_SEQ_Evt_RadioOnTstRF,
  CFG_SEQ_Evt_NBR
} CFG_SEQ_IdleEvt_Id_t;


extern volatile uint32_t RadioTxDone_flag;

#endif
