- [1. This LoRaWAN Application](#1-this-lorawan-application)
  - [1.1. Presentation](#11-presentation)
  - [1.2. Running the application](#12-running-the-application)
    - [1.2.1. Board test](#121-board-test)
    - [1.2.3. Application configuration](#123-application-configuration)
    - [1.2.4. Possible action during excecution](#124-possible-action-during-excecution)
- [2. LoRaWAN Application layer Advanced features](#2-lorawan-application-layer-advanced-features)
  - [2.1. Clock Synchronization](#21-clock-synchronization)
  - [2.2. Multicast](#22-multicast)
  - [2.3. Fragmentation](#23-fragmentation)
- [3. Want to learn more about LoRaWAN ?](#3-want-to-learn-more-about-lorawan-)



# 1. This LoRaWAN Application
## 1.1. Presentation
This standalone project is a single-core application including a **1.0.3 LoRaWAN stack**. This project is inspired from ST "End-Node" example (see STM32Cube MCU Package for STM32WL series). The  STM32CubeIDE. 

:heavy_check_mark: A single file is used to configure the application behavior and setup all LoRaWAN settings (Root Keys or Sessions Keys).

:heavy_check_mark: This file is **/LoRaWAN/config_application.h**.

:heavy_check_mark: This project works on a **NUCLEO-WL55JC1 board**.

:heavy_check_mark: The user payload is a simulated temperature and humidity sensor. But this application can provide real temperature and humidity value if you plug the **IKS01A3 board**. To switch between simulated value and real value, set the **ADMIN_SENSOR_ENABLED** to **true** in the **LoRaWAN/App/General_Setup.h** file.

## 1.2. Running the application

:warning: The only file to modify in order to run this project is **/LoRaWAN/config_application.h**. See details in the next chapter.


| #define           	| Possible values      	|  Behavior                                                    	|
|----------------------	|--------------------	|---------------------------------------------------------------|
| ACTIVATION_MODE      	| ABP or OTAA        	|  Selection of Activation Method                              	|
| CLASS                	| CLASS_A or CLASS_C 	|  Class selection                                             	|
| SPREADING_FACTOR     	| Number [7;12]      	|  7=SF7, 8=SF8, ..., 12=SF12                                  	|
| ADAPTIVE_DR          	| true or false      	|  Enable Adaptive Data Rate (if true)                         	|
| CONFIRMED            	| true or false      	|  Frame Confirmed (if true) OR Frame Unconfirmed (if false)   	|
| APP_PORT             	| Number [0;255]     	|  Application Port number                                     	|
|                      	|                    	|                                                              	|
| SEND_BY_PUSH_BUTTON  	| true or false      	|  Sending method (Time or Push Button)                        	|
| FRAME_DELAY          	| Time in ms         	|  Time between 2 frames (Minimum 7000)                        	|
| PAYLOAD_1234        	| true or false      	|  Sends 0x01 0x02 0x03 0x04               	|
| PAYLOAD_TEMPERATURE  	| true or false      	|  Sends a temperature between 20 and 25 degrees     			|
| PAYLOAD_HUMIDITY  	| true or false      	|  Sends a humidity (%) between 0 and 100		     			|
| CAYENNE_LPP_         	| true or false      	|  Enable all sensors and use the CAYENNE LPP format (if true) 	|
| LOW_POWER            	| true or false      	|  Enable Low Power mode between two frames (if true)          	|


### 1.2.1. Board test
1. Plug the board to your computer thanks to a micro-USB cable.
2. A new drive has been mounted on your Filesystem (E:\ or F:\ or ...). Droping a executable file in this drive will automatically program you STM32WL MCU.
3. Open a serial terminal : 115200 bps - 8 bits - 1 stop bit. On the serial link, you should see a confirmation telling you that the board has been successfully programmed.
4. Copy-paste the file \STM32WL\helloworld.bin in STM32WL drive to program your MCU. Reset the MCU.
 
 
### 1.2.3. Application configuration
:warning: There must be no space in the project path. 

:one: **STM32CubeIDE installation**
* STM32CubeIDE and board's drivers can be downloaded from ST's website
* To open the project, double click on the file **STM32WL-standalone\.project**.
	
The configuration of the project is done with the file /LoRaWAN/config_application.h and configure the following project parameters:

:two: **Configuration of the LoRaWAN information**
* The activation mode (ACTIVATION_MODE)
* The class (CLASS)
* The spreading factor (SPREADING_FACTOR).
* The type of frame (CONFIRMED)
* The application port (APP_PORT)

:three: **Configuration of the application payload:**
This payloads can be:
* 0x01 0x02 0x03 0x04 (if PAYLOAD_1234 is true)
* 1 byte temperature (if PAYLOAD_TEMPERATURE is true)
* 1 byte humidity (if PAYLOAD_HUMIDITY is true)
Temperature and Humidity payload can be combined in the same frame (2 bytes) if both PAYLOAD_TEMPERATURE and PAYLOAD_HUMIDITY are true.

* The Cayenne LPP payload format can be used if CAYENNE_LPP_ is true.

:four: **Choose the way you send data**   
* use the B3 push button (if SEND_BY_PUSH_BUTTON is true)
**or**
* use a period of FRAME_DELAY ms (if SEND_BY_PUSH_BUTTON is false).

:five: **Select the low power feature**

:six: **Enter the LoRaWAN Keys and EUI**
* devEUI, devAddr, nwkSKey and appSKey for an ABP end-device.
* devEUI, appKeys and appEUI in case of OTAA end-device. 

:seven:  **Open a serial terminal**
* 115200 bps - 8 bits - 1 stop bit.
 
:eight: **Run your code**
* On the serial link, you should see the application logs.

### 1.2.4. Possible action during excecution
During execution, we can provide commands. By typing the following letters on your keyboard, you will :
* **T**ransmit a new frame if you type 't'
* **R**eset the board if you type 'r'
* Display the **H**elp menu if you type 'h'
* Enter the **LoRa** raw packet application if you type 'lora'

The LoRa raw packet application let the user send raw LoRa packet (not LoRaWAN). You can also directly enter this application by pressing the user button B2 during Reset.

# 2. LoRaWAN Application layer Advanced features
In addition to the previous application, this project also provide **Clock Synchronization**, **Multicast** and **Fragmentation** messaging packages activated. Please contact us if you want to use the clock Synchronisation application dashboard, the Multicast dashboard and the Fragmentation dashboard.

## 2.1. Clock Synchronization
* Package version implemented:          1
* Dedicated LoRaWAN port:               202

## 2.2. Multicast
* Package version implemented:          1
* Dedicated LoRaWAN port:               200

## 2.3. Fragmentation
* Package version implemented:          1
* Dedicated LoRaWAN port:               201
* Fragmentation algorithm implemented:  FEC (Forward Error Correction)
* Available fragmentation mode:         Interop (This mode represents the minimum test proving that the end device is able to receive a data block file from the server.
                                        It doesn’t allow you to perform FUOTA)
* Added modification:                   The ST package implements the version 1 of the Fragmented data block layer messaging package. That means there is no _FragDataBlockReceivedReq_ command used by the end-device to signal the fragmentation server that a data block has been completely received (command only offered by the version 2 of the package). A dedicated function has been added. When the whole data block has been rebuilt, `SendTXStopFrag()` sends 0x0001 on port 186. Payload and port are arbitrary chosen and can be adjusted. To stop sending fragments, your fragmentation server should be able to understand this message.

# 3. Want to learn more about LoRaWAN?
Savoie Mont Blanc University provides many educationnal ressources about LoRaWAN:

:notebook: One free ebook available here:[LoRaWAN for beginers books](https://www.univ-smb.fr/lorawan/en/free-book/)

:tv: E-learning platform available here: [LoRaWAN for beginers videos](https://www.udemy.com/course/lora-lorawan-internet-of-things/?referralCode=21DED0F1021F4E261955)

:tv: E-learning platform for Advanced users here: [LoRaWAN for Advanced users videos](https://www.udemy.com/course/lorawan-for-advanced-users/?referralCode=BA4A670560916E1AED77)

:bulb: 2 days training sessions available here: [LoRaWAN and IoT Training](https://www.univ-smb.fr/lorawan/avada_portfolio/formation-distanciel/)

