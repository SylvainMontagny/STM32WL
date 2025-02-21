/**
  ******************************************************************************
  * @file    General_Setup.h
  * @author  Antoine AUGAGNEUR, USMB LoRaWAN Team
  * @brief   Various definition for project setup
  * @Date    avr. 2022
  *
  * Macro definition linked with files: se-identity.h, lora_app.c, lora_app.h, sys_conf.h, soft-se.c
  *
  * Logs:
  * --> Starting:		lora_app.c
  * --> Key printing:	soft-se.c
  * --> Frame sent:		lora_app.c
  * --> Frame received:	lora_app.c
  *
  *
  ******************************************************************************
*/

#ifndef APPLICATION_USER_LORAWAN_GENERAL_SETUP_H_
#define APPLICATION_USER_LORAWAN_GENERAL_SETUP_H_


#define ABP				1
#define OTAA			2

#define true			1
#define false	 		0



/*---------------------------------------------------------------------------------------------------------------
 ACTIVATION_MODE     |    ABP or OTAA     |    # Selection of Activation Method                                  |
 CLASS               | CLASS_A or CLASS_C |    # Class selection												 |
 SPREADING_FACTOR    |   Number [7;12]    |    # 7=SF7, 8=SF8, ..., 12=SF12                                      |
 ADAPTIVE_DR         |  true or false     |    # Enable Adaptive Data Rate (if true)                             |
 CONFIRMED           |  true or false     |    # Frame Confirmed (if true) OR Frame Unconfirmed (if false)       |
 PORT                |   Number [0;255]   |    # Application Port number                                         |
                       	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	   	 |
 SEND_BY_PUSH_BUTTON |   true or false    |    # Sending method (Time or Push Button)                            |
 FRAME_DELAY         |    Time in ms      |    # Time between 2 frames (Minimum 7000)                            |
 PAYLOAD_1234        |  true or false     |    # Sends 0x01 0x02 0x03 0x04                   |
 PAYLOAD_TEMPERATURE |  true or false     |    # Sends a simulated temperature between 20 and 25 degrees         |
 CAYENNE_LPP_        |  true or false     |    # Enable all sensors and use the CAYENNE LPP format (if true)     |
 LOW_POWER           |  true or false     |    # Enable Low Power mode between two frames (if true)              |
---------------------------------------------------------------------------------------------------------------*/

#include "../config_application.h"


/* LoRaWAN Activation Mode - Class---------------------------------------------*/
#define ADMIN_ACTIVATION_TYPE			ACTIVATION_MODE		// possible values: ACTIVATION_TYPE_OTAA / ACTIVATION_TYPE_ABP - lora_app.h
#define ADMIN_DEFAULT_CLASS				CLASS				// possible values: CLASS_A / CLASS_B / CLASS_C - lora_app.h

/* LoRaWAN Credentials --------------------------------------------------------*/

// OTAA Keys --
#define ADMIN_OTAA_DEV_EUI				devEUI_ 	// se-identity.h
#define ADMIN_OTAA_APP_EUI				appEUI_ 	// se-identity.h
#define ADMIN_OTAA_APP_KEY				appKey_ 	// se-identity.h


// ABP Keys --
#define ADMIN_ABP_DEV_ADDR				devAddr_ 	// se-identity.h
#define ADMIN_ABP_NWK_S_KEY				nwkSKey_ 	// se-identity.h
#define ADMIN_ABP_APP_S_KEY				appSKey_ 	// se-identity.h


/* LoRaWAN Transmission settings --------------------------------------------*/
#define ADMIN_DEFAULT_DATA_RATE_IF_NOT_ADR			ADMIN_DR	// possible values: DR_0 / DR_1 / DR_2 / DR_3 / DR_4 / DR_5 - lora_app.h
#define ADMIN_DEFAULT_DATA_RATE_IF_ADR				ADMIN_DR	// possible values: DR_0 / DR_1 / DR_2 / DR_3 / DR_4 / DR_5 - RegionEU868.h
#define ADMIN_ADR_STATE								ADAPTIVE_DR	// possible values: LORAMAC_HANDLER_ADR_ON / LORAMAC_HANDLER_ADR_OFF
#define ADMIN_RECEIVE_DELAY1		1000								// RegionCommon.h
#define ADMIN_RX2_DR				DR_0								// possible values: DR_0 to DR_5 - RegionEU868.h
#define ADMIN_ADR_ACK_LIMIT			8									// Default value=64. RegionCommon.h
#define ADMIN_ADR_ACK_DELAY			4									// Default value=32. RegionCommon.h
#define ADMIN_MAX_ACK_RETRIES		8									// Number of retries when using confirmed packet. Default value=8. LoRaMac.h

/* LoRaWAN Application --------------------------------------------------------*/
#define RAW_LORA_APP					false

// Uplink Mode
#define ADMIN_TX_TYPE							SEND_BY_PUSH_BUTTON				// possible values: 0 (by timer, every TxDutyCycleTime) / 1 (by button B3) - lora_app.c
#define ADMIN_TxDutyCycleTime					FRAME_DELAY						// in ms - lora_app.h
#define ADMIN_TX_CONFIRMED_TYPE					CONFIRMED						// possible values: LORAMAC_HANDLER_UNCONFIRMED_MSG / LORAMAC_HANDLER_CONFIRMED_MSG - lora_app.h


// Power
#define ADMIN_LOW_POWER					!LOW_POWER				// 0 (Low Power enabled) / 1 (Low Power disabled)

// Sensors
#define ADMIN_SENSOR_ENABLED			false					// 0 (No sensors) / 1 (Sensors Board IKS01A3)
#define USMB_VALVE						    true
#define VALVE_APP_PORT					  30

/* Device LOGs ----------------------------------------------------------------*/
#define ADMIN_VERBOSE					VLEVEL_L				// possible values: VLEVEL_H / VLEVEL_M / VLEVEL_L - Utilities-Conf.h
#define ADMIN_KEY_EXTRACT				1						// possible values: 1 (display all keys), 0 (none) - lorawan_conf.h


/* LoRaWAN Multicast ----------------------------------------------------------*/
#define ADMIN_GEN_APP_KEY			    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00 		// se-identity.h


/***** Escape Caracters *****/
#define TERMINAL_COLORS		true
#define RESET_COLOR			"\033[0m"			// Reset all attribute
#define CURSOR_TOP_LEFT		"\033[0;0H"			// Cursor back to Top-Left
#define CLEAR_SCREEN		"\033[0;0H\033[2J"	// Clear Screen
#define BLACK				"\033[30m" 			// Black
#define RED					"\033[31m"			// Red
#define GREEN				"\033[32m"			// Green
#define YELLOW  			"\033[33m"      	/* Yellow */
#define BLUE    			"\033[34m"      	/* Blue */
#define MAGENTA 			"\033[35m"      	/* Magenta */○
#define CYAN    			"\033[36m"      	/* Cyan */
#define WHITE   			"\033[37m"      	/* White */
#define BOLDBLACK   		"\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     		"\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   		"\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  		"\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    		"\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA 		"\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    		"\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   		"\033[1m\033[37m"      /* Bold White */



#if (ACTIVATION_MODE != OTAA) && (ACTIVATION_MODE != ABP)
	#error "ACTIVATION_MODE must be either OTAA or APB in your config_application.h file"
#endif

#if (CLASS != CLASS_A) &&  (CLASS != CLASS_C)
	#error "CLASS must be either CLASS_A or CLASS_C in your config_application.h file"
#endif

#if (SEND_BY_PUSH_BUTTON != true) &&  (CLASS != false)
	#error "SEND_BY_PUSH_BUTTON must be either true of false in your config_application.h file"
#endif

#if FRAME_DELAY < 8000
	#error "FRAME_DELAY must be greater than 8000 ms in your config_application.h file"
#endif

#if (SPREADING_FACTOR == 12)
#define ADMIN_DR	DR_0

#elif (SPREADING_FACTOR == 11)
#define ADMIN_DR	DR_1

#elif (SPREADING_FACTOR == 10)
#define ADMIN_DR	DR_2

#elif (SPREADING_FACTOR == 9)
#define ADMIN_DR	DR_3

#elif (SPREADING_FACTOR == 8)
#define ADMIN_DR	DR_4

#elif (SPREADING_FACTOR == 7)
#define ADMIN_DR	DR_5

#else
	#error "Wrong SPREADING_FACTOR definition in your config_application.h file"
#endif

#if (ADAPTIVE_DR != true) && (ADAPTIVE_DR != false)
	#error "ADAPTIVE_DR must be either true of false in your config_application.h file"
#endif

#if (CONFIRMED != true) && (CONFIRMED != false)
	#error "CONFIRMED must be either true of false in your config_application.h file"
#endif

#if (APP_PORT < 1) || (APP_PORT > 255)
	#error "PORT must be between 1 and 255 in your config_application.h file"
#endif


#if(PAYLOAD_1234)
	#if (PAYLOAD_TEMPERATURE || PAYLOAD_HUMIDITY || CAYENNE_LPP_)
		#error "PAYLOAD_TEMPERATURE or PAYLOAD_HUMIDITY or CAYENNE_LPP_ can't be enable when PAYLOAD_1234 is enable in your config_application.h file"
	#endif
#endif

#if(USMB_VALVE)
	#if (PAYLOAD_1234 || PAYLOAD_TEMPERATURE || PAYLOAD_HUMIDITY || CAYENNE_LPP_)
		#error "PAYLOAD_1234 or PAYLOAD_TEMPERATURE or PAYLOAD_HUMIDITY or CAYENNE_LPP_ can't be enable when USMB_VALVE is enable in your config_application.h file"

	#endif
#endif


#if (LOW_POWER != true) && (LOW_POWER != false)
	#error "LOW_POWER must be either true of false in your config_application.h file"
#endif



#endif /* APPLICATION_USER_LORAWAN_GENERAL_SETUP_H_ */



