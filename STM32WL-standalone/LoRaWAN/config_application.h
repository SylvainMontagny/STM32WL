
#define ACTIVATION_MODE     		OTAA
#define CLASS						CLASS_A
#define SPREADING_FACTOR    		8
#define ADAPTIVE_DR         		false
#define CONFIRMED           		false
#define PORT                		15

#define SEND_BY_PUSH_BUTTON 		true
#define FRAME_DELAY         		10000
#define PAYLOAD_HELLO				true
#define PAYLOAD_TEMPERATURE    		false
#define CAYENNE_LPP_         		false
#define LOW_POWER           		false


#define devEUI_						{ 0x00, 0x80, 0xE1, 0x15, 0x00, 0x0A, 0x9B, 0xD2 }
//#define devEUI_						{ 0x00, 0x80, 0xE1, 0x15, 0x00, 0x0A, 0x96, 0x8E }
//00 80 E1 15 00 0A 9B D2


// Configuration for ABP Activation Mode
#define devAddr_ 					( uint32_t )0x260BBBD3
#define nwkSKey_ 					37,3B,A9,06,ED,0C,9E,AB,45,76,7A,C4,F0,45,48,09
#define appSKey_ 					DA,20,76,8C,71,83,F2,01,1F,41,EF,63,7C,A7,08,14


// Configuration for OTAA Activation Mode
//#define appKey_						23,09,8A,A0,C9,1D,EF,A0,49,4C,14,2E,72,92,3A,3E
#define appKey_							BC, 67, 3B, 8F, 44, 31, AC, E4, A7, B8, 41, 22, 09, AE, 1C, 45
#define appEUI_						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
