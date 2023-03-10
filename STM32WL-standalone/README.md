

# 1. LoRaWAN Application
## 1.1. Presentation
This standalone project is a single-core application inspired by ST "End-Node" example (see STM32Cube MCU Package for STM32WL series). The source code has been mofified with STM32CubeIDE. 

:heavy_check_mark: A single file is used to configure the application behavior and setup all LoRaWAN settings (Root Keys or Sessions Keys). 
:heavy_check_mark: This file is : **/LoRaWAN/config_application.h**
:heavy_check_mark: This project works on a **NUCLEO-WL55JC1 board**.

## 1.2. Running the application
:warning: The only file to modify in order to run this project is : **/LoRaWAN/config_application.h**. See details in the next chapter.
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


### 1.2.1. Board test
1. Plug the board to your computer thanks to a micro-USB cable.
2. A new drive has been mounted on your Filesystem (E:\ or F:\ or ...). Droping a executable file in this drive will automatically program you STM32WL MCU.
3. Copy-paste the file \STM32WL\helloworld.bin in STM32WL drive to program your MCU.
4. Open a serial terminal : 115200 bps - 8 bits - 1 stop bit. On the serial link, you should see a confirmation telling you that the board has been successfully programmed.
 
### 1.2.3. Application configuration
:one: **STM32CubeIDE installation**
	* STM32CubeIDE and drivers can be downloaded from ST's website
	* To open the project, double click on the file **STM32WL-standalone\.project**.
	
The configuration of the project is done with the file /LoRaWAN/config_application.h and configure the following project parameters:

:two: **Configuration the LoRaWAN information**
    * The activation mode (ACTIVATION_MODE)
    * The class (CLASS)
    * The spreading factor (SPREADING_FACTOR). The default Bandwidth is 125 kHz.
    * The type of frame (CONFIRMED)
    * The application port (PORT)

:three: **Configuration of the application payload:**
This payloads can be:
    * HELLO String (if PAYLOAD_HELLO is enable) 
    **or**
    * 1 byte simulated temperature (if PAYLOAD_TEMPERATURE is enable)
    **or**
    * all sensors payload in a Cayenne LPP format (if CAYENNE_LPP_ is enable).

:four: **Choose the way you send data**   
    * use the B3 push button (SEND_BY_PUSH_BUTTON = true)
    **or**
    * uSe a period of FRAME_DELAY ms (when SEND_BY_PUSH_BUTTON = false).

:five: **Select the low power feature**

:six: **Enter the LoRaWAN Keys and EUI**
    * devEUI, devAddr, nwkSKey and appSKey for an ABP end-device.
    * devEUI, appKeys and appEUI in case of OTAA end-device. 

:seven:  ** Open a serial terminal **
	* 115200 bps - 8 bits - 1 stop bit.
 
:eight: ** Run your code **
	* On the serial link, you should see the application running.

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

