#define ACTIVATION_MODE     		ABP
#define CLASS						CLASS_A
#define SPREADING_FACTOR    		7
#define ADAPTIVE_DR         		false
#define CONFIRMED           		false
#define PORT                		15

#define SEND_BY_PUSH_BUTTON 		false
#define FRAME_DELAY         		10000
#define PAYLOAD_HELLO				true
#define PAYLOAD_TEMPERATURE    		false
#define CAYENNE_LPP_         		false
#define LOW_POWER           		false


#define devEUI_						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// Configuration for ABP Activation Mode
#define devAddr_ 					( uint32_t )0x00000000
#define nwkSKey_ 					00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
#define appSKey_ 					00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00


// Configuration for OTAA Activation Mode
#define appKey_						00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
#define appEUI_						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }