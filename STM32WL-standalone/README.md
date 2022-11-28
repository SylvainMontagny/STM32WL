
# 1. LoRaWAN Application
## Presentation
This standalone project is a single-core application inspired by ST "End-Node" example (see STM32Cube MCU Package for STM32WL series). The source code has been mofified with STM32CubeIDE. A single file is used to configure the application behavior and setup all LoRaWAN settings (Root Keys or Sessions Keys). This file is : **/LoRaWAN/config_application.h**
This project shall be used with a **NUCLEO-WL55JC1 board**.

## Run the application
The only file to modify to run this project is : **/LoRaWAN/config_application.h**
| #define           	| Possible values      	|  Behavior                                                    	|
|----------------------	|--------------------	|---------------------------------------------------------------|
| ACTIVATION_MODE      	| ABP or OTAA        	|  Selection of Activation Method                              	|
| CLASS                	| CLASS_A or CLASS_C 	|  Class selection                                             	|
| SPREADING_FACTOR     	| Number [7;12]      	|  7=SF7, 8=SF8, ..., 12=SF12                                  	|
| ADAPTIVE_DR          	| true or false      	|  Enable Adaptive Data Rate (if true)                         	|
| CONFIRMED            	| true or false      	|  Frame Confirmed (if true) OR Frame Unconfirmed (if false)   	|
| PORT                 	| Number [0;255]     	|  Application Port number                                     	|
|                      	|                    	|                                                              	|
| SEND_BY_PUSH_BUTTON  	| true or false      	|  Sending method (Time or Push Button)                        	|
| FRAME_DELAY          	| Time in ms         	|  Time between 2 frames (Minimum 7000)                        	|
| PAYLOAD_HELLO        	| true or false      	|  Sends the string HELLO : 48 45 4C 4C 4F (hex)               	|
| PAYLOAD_TEMPERATURE  	| true or false      	|  Sends a simulated temperature between 20 and 25 degrees     	|
| CAYENNE_LPP_         	| true or false      	|  Enable all sensors and use the CAYENNE LPP format (if true) 	|
| LOW_POWER            	| true or false      	|  Enable Low Power mode between two frames (if true)          	|

1. **Open the project in you favorite IDE**

2. **Configure the LoraWAN information:**
    * The activation mode (ACTIVATION_MODE)
    * The class (CLASS)
    * The spreading factor (SPREADING_FACTOR). The default Bandwidth is 125 kHz.
    * The type of frame (CONFIRMED)
    * The application port (PORT)

3. **Configure the application payload:**
This payloads can be:
    * HELLO String (if PAYLOAD_HELLO is enable) 
    **or**
    * 1 byte simulated temperature (if PAYLOAD_TEMPERATURE is enable)
    **or**
    * all sensors payload in a Cayenne LPP format (if CAYENNE_LPP_ is enable).

4. **Choose the way you send data:**   
    * use the B3 push button (SEND_BY_PUSH_BUTTON = true)
    **or**
    * uSe a period of FRAM_DELAY ms (when SEND_BY_PUSH_BUTTON = false).

5. **Select if you want the low power modes enable( LOW_POWER = true)**

6. **Enter the LoRaWAN Keys and EUI**
    * devEUI, devAddr, nwkSKey and appSKey for an ABP end-device.
    * devEUI, appKeys and appEUI in case of OTAA end-device. 


# 2. LoRaWAN Application layer Advanced features
In addition to the previous application, this project also provide **Clock Synchronization**, **Multicast** and **Fragmentation** messaging packages activated. 

## Clock Synchronization
- Package version implemented:          1
- Dedicated LoRaWAN port:               202

## Multicast
- Package version implemented:          1
- Dedicated LoRaWAN port:               200

## Fragmentation
- Package version implemented:          1
- Dedicated LoRaWAN port:               201
- Fragmentation algorithm implemented:  FEC (Forward Error Correction)
- Available fragmentation mode:         Interop (This mode represents the minimum test proving that the end device is able to receive a data block file from the server.
                                        It doesn’t allow you to perform FUOTA)
- Added modification:                   The ST package implements the version 1 of the Fragmented data block layer messaging package. That means there is no _FragDataBlockReceivedReq_ command used by the end-device to signal the fragmentation server that a data block has been completely received (command only offered by the version 2 of the package). A dedicated function has been added. When the whole data block has been rebuilt, `SendTXStopFrag()` sends 0x0001 on port 186. Payload and port are arbitrary chosen and can be adjusted. To stop sending fragments, your fragmentation server should be able to understand this message.

# 3. Want to learn more about LoRaWAN ?
- [LoRaWAN for Advanced Users books](https://www.univ-smb.fr/lorawan/en/free-book/)
- [LoRaWAN for beginers videos](https://www.udemy.com/course/lora-lorawan-internet-of-things/?referralCode=21DED0F1021F4E261955)

