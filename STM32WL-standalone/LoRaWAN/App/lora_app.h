


#ifndef __LORA_APP_H__
#define __LORA_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include  "General_Setup.h"

/* LoraWAN application configuration (Mw is configured by lorawan_conf.h) */
#define ACTIVE_REGION                               LORAMAC_REGION_EU868
#define CAYENNE_LPP									CAYENNE_LPP_														// see General_Setup.h
//#define APP_PERSO									ADMIN_APP_PERSO														// see General_Setup.h
#define APP_TX_DUTYCYCLE                            ADMIN_TxDutyCycleTime												// see General_Setup.h
#define LORAWAN_USER_APP_PORT                       APP_PORT
#define LORAWAN_SWITCH_CLASS_PORT                   3
#define LORAWAN_DEFAULT_CLASS                       ADMIN_DEFAULT_CLASS													// see General_Setup.h
#define LORAWAN_DEFAULT_CONFIRMED_MSG_STATE         ADMIN_TX_CONFIRMED_TYPE												// see General_Setup.h
#define LORAWAN_ADR_STATE                           ADMIN_ADR_STATE														// see General_Setup.h
#define LORAWAN_DEFAULT_DATA_RATE                   ADMIN_DEFAULT_DATA_RATE_IF_NOT_ADR									// see General_Setup.h
#define LORAWAN_DEFAULT_ACTIVATION_TYPE             ADMIN_ACTIVATION_TYPE												// see General_Setup.h
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/*!
 * Default Unicast ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       4

void LoRaWAN_Init(void);

#ifdef __cplusplus
}
#endif

#endif
