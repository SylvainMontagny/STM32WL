
# STM32WL Advanced features

This standalone project is a single-core project inspired by an End Node project provided by ST (see STM32Cube MCU Package for STM32WL series).
This project is a classic end-node program with Clock Synch. & Multicast & Fragmentation messaging packages activated. **/!\ The Firmware management package is not implemented.**

## End-Node basic configurations & information

### End-Node project
This project shall be used with a NUCLEO-WL55JC1 board.

### End-Node setup
A single file is used to setup all the LoRaWAN settings (Keys, Class, Activation method, etc.).
- The file:     General_Setup.h
- Its location: STM32WL-standalone\LoRaWAN\App

## End-Node messaging packages

### Clock Synchronization
- Package version implemented:          1
- Dedicated LoRaWAN port:               202

### Multicast
- Package version implemented:          1
- Dedicated LoRaWAN port:               200

### Fragmentation
- Package version implemented:          1
- Dedicated LoRaWAN port:               201
- Fragmentation algorithm implemented:  FEC (Forward Error Correction)
- Available fragmentation mode:         Interop (This mode represents the minimum test proving that the end device is able to receive a data block file from the server.
                                        It doesn’t allow you to perform FUOTA)
- Added modification:                   The ST package implements the version 1 of the Fragmented data block layer messaging package. That means there is no _FragDataBlockReceivedReq_ command used by the end-device to signal the fragmentation server that a data block has been completely received (command only offered by the version 2 of the package). A dedicated function has been added. When the whole data block has been rebuilt, `SendTXStopFrag()` sends 0x0001 on port 186. Payload and port are arbitrary chosen and can be adjusted. To stop sending fragments, your fragmentation server should be able to understand this message.

## Want to learn more about LoRaWAN ?
- [LoRaWAN for Advanced Users book](https://www.univ-smb.fr/lorawan/en/free-book/)
- [LoRaWAN for beginers videos](https://www.udemy.com/course/lora-lorawan-internet-of-things/?referralCode=21DED0F1021F4E261955)

