
#define ACTIVATION_MODE     		OTAA
#define CLASS						CLASS_A
#define SPREADING_FACTOR    		7
#define ADAPTIVE_DR         		false
#define CONFIRMED           		false
#define APP_PORT                	15

#define SEND_BY_PUSH_BUTTON 		true
#define FRAME_DELAY         		10000
#define PAYLOAD_HELLO				false
#define PAYLOAD_TEMPERATURE    		true
#define PAYLOAD_HUMIDITY			false
#define CAYENNE_LPP_         		false
#define LOW_POWER           		false

//#define devEUI_						{ 0x00, 0x80, 0xE1, 0x15, 0x00, 0x0A, 0x9B, 0xD2 }
// capaz-holin __
//#define devEUI_						{ 0x06,0x10,0x73,0x0c,0xd0,0x6a,0x08,0x70 }
// #define devEUI_							{ 0x00, 0x80, 0xe1, 0x15, 0x00, 0x0a, 0x9e, 0xef }



// Configuration for ABP Activation Mode
//#define devAddr_ 					( uint32_t )0x260BBBD3
//#define nwkSKey_ 					37,3B,A9,06,ED,0C,9E,AB,45,76,7A,C4,F0,45,48,09
//#define appSKey_ 					DA,20,76,8C,71,83,F2,01,1F,41,EF,63,7C,A7,08,14
#define devAddr_ 					( uint32_t )0xfc0148cb
#define nwkSKey_ 					83,d2,46,50,40,0c,5a,0a,b9,0a,2e,03,39,cd,1e,05
#define appSKey_ 					9a,2d,28,68,34,61,e7,7f,5b,4b,b3,e8,c3,b2,a9,8e


// Configuration for OTAA Activation Mode

// device1-tpc
//#define devEUI_						{ 0xf0,0x35,0xf9,0x33,0xa6,0xb9,0x92,0x2f }
//#define appKey_						1a,13,e8,9f,26,af,3f,e5,d6,58,66,70,45,d8,1f,09
//#define appEUI_						{ 0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00 }

// device1-tpc-js
#define devEUI_							{ 0x0A, 0xB4, 0x23, 0x09, 0x06, 0x04, 0x88, 0x61 }
#define appKey_						20,30,C0,B5,5D,6D,C6,25,28,A5,9A,B1,16,02,F9,0C
#define appEUI_						{ 0xF0, 0x3D, 0x29, 0xAC, 0x71, 0x01, 0x00, 0x01 }
//C66BCF1C9B7AECDD85A9C618EC5857A3


// Helium
//#define appKey_						C6,BE,38,67,97,B2,2D,A8,C5,29,C1,FD,09,F1,94,D1
//#define appEUI_						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }


// Chirpstack server 1
//#define devEUI_						{ 0x84, 0xae, 0x7c, 0x74, 0x0f, 0x3d, 0x5c, 0x18 }
//#define appKey_						bb,88,e7,b2,57,60,da,65,5b,52,b0,67,6c,a5,ce,ef
//#define appEUI_						{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }



