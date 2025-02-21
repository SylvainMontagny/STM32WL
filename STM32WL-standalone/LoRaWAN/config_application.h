#define ACTIVATION_MODE     		OTAA
#define CLASS						CLASS_A
#define SPREADING_FACTOR    		7
#define ADAPTIVE_DR         		false
#define CONFIRMED           		false
#define APP_PORT                	15

#define SEND_BY_PUSH_BUTTON 		false
#define FRAME_DELAY         		10000
#define PAYLOAD_1234				false
#define PAYLOAD_TEMPERATURE    		false
#define PAYLOAD_HUMIDITY   			false
#define CAYENNE_LPP_         		false
#define LOW_POWER           		false


#define devEUI_						{ 0xec, 0xdb, 0x86, 0xff, 0xfd, 0x52, 0xc9, 0x83 }

// Configuration for ABP Activation Mode
#define devAddr_ 					( uint32_t )0x00000000
#define nwkSKey_ 					00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
#define appSKey_ 					00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00


// Configuration for OTAA Activation Mode
#define appKey_						ca,4c,a3,a2,42,42,2f,e9,74,88,14,fd,76,99,50,28
#define appEUI_						{ 0x4f, 0x17, 0xa1, 0x25, 0xcb, 0xca, 0xeb, 0x33 }
