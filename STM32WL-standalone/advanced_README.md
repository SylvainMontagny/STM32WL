# Readme for advanced configurations

Advanced configurations are mainly defined in `LoRaWAN/App/General_Setup.h`. Most of them are already modified by the `config_application.h` file.

## Print logs on NUCLEO-GFX01M2 extension LCD screen

First plug the screen on top of the NUCLEO-STM32WL55JC board, the blue screen joystick on the same side of the USB port and the other buttons.

| #define          |Possible values | Behaviour |
|------------------|----------------|-----------|
| LCD_DISPLAY      | true or false  | Send logs to the screen and display them. Those logs can be different from serial logs and are less verbose.
| DISPLAY_NB_LINES | true or false  | Add line number to the logs.

Other settings can be modified in `Drivers\Inc\lcd_printf.h` to customize the logs printed on the lcd screen.

## Device simulations

If none of the basic configurations are selected (i.e. PAYLOAD_1234, PAYLOAD_TEMPERATURE, PAYLOAD_HUMIDITY or CAYENNE_LPP_), it is possible to simulate another device with a real simulated payload.

| #define          |Possible values | Behaviour |
|------------------|----------------|-----------|
| USMB_VALVE       | true or false  | Thermostatic valve simulation with temperature and setpoint data. |
| VALVE_APP_PORT   | Number [0;255] | Port for downlink configuration. Default 30. |
| ATIM_THAQ        | true or false  | Temperature variations. |
| WATTECO_TEMPO    | true or false  | Raw payload. |
| WATTECO_TEMPO_PORT | Number [0;255] | Port for payload decoder. Default 125. |
| TCT_EGREEN       | true or false  | Courant, voltage (and eventually temperature) variations. |