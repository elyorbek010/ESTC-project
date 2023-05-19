# Embedded Systems Laboratory Final Assignment

This program is designed to control a LED light on board pca10059 nrf52840 Dongle, using various methods such as multifunctional button, command line interface, and BLE characteristics. The program also includes a persistent data storage system that saves the color of the LED, retrieving the color after a power cycle. Additionally, this program allows you to save up to 10 preferred colors using the command line interface, and then apply them whenever you want.

## Usage

To use this program, first ensure that you have the required hardware. You will need the pca10059 nrf52840 board.  
To compile the program you will need to have an nRF SDK 17, Make, nRF Util, Compiler arm-non-eabi-gcc, nRF Connect, and an application PuTTy or alternatively picocom.

## Build
In the folder armgcc type
>make clean - to remove all executable files from a program binary and coding directory.  
>make SDK_ROOT='PATH TO nRF SDK' - show the path to the SDK  
>make ESTC_USB_CLI_ENABLED=0, if you want to build the app without Command Line Interface on, it is enabled by default.  
>make dfu - to update the device firmware

### Multifunctional Button

The onboard multifunctional button can be used to turn the LED on and off. Press the button once to turn it on, and press it again to turn it off.
The onboard multifunctinoal button can be used to change the LED colors. Double Click(interval <500ms) the button to change the operating mode, and the Long Press(interval >500ms) can change the LED color.
There are 4 modes:  
```
NO INPUT -- default mode, nothing happens
HUE MODIFICATION -- in long press, change hue from 0° to 360° and back and forth
SATURATION MODIFICATION -- in long press, change saturation from 0% to 100% and back and forth
VALUE MODIFICATION -- in long press, change the brightness from 0% to 100% and back and forth
```
The speed of the slider of HSV values can be set in hsv_rgb.h  


### Command Line Interface

The command line interface allows you to control the LED light using your computer terminal. Connect your board to your computer using a USB port, and open a terminal window.
You can use the application PuTTy to connect to the Serial USB Terminal or terminal based application picocom.  
There are many commands you can try, they are used just like linux terminal commands:

```
RGB <red> <green> <blue> - the device sets current color to specified one.  
HSV <hur> <saturation> <value> - the same with RGB, but color is specified in HSV   
add_rgb_color <r> <g> <b> <color_name> - adds color with color_name to the database, max 10 color preferences are supported
add_current_color <color_name> - get the value of the current color LED is lighting and save it under name color_name
del_color <color_name> - delete the color from the database
apply_color <color_name> - sets the value of the LED to the saved color with name color_name 
list_colors - list all colors saved in the database
help - print information about supported commands
```

### BLE Characteristics

The program includes two BLE characteristics: one for writing data, and one for reading and receiving notifications of changes in the LED color value.

To connect your BLE-enabled device to the board, search for and connect to the device with the name "Elyorbek" using the application nRF Connect. Once connected, you can write data to the characteristic with UUID `16F52068-4D4C-4FFE-91F6-C387F501C9F9` to change the LED color. You can also read and receive notifications from the characteristic with UUID `16F52069-4D4C-4FFE-91F6-C387F501C9F9`.

## Persistent Data Storage

The program includes a persistent data storage system that saves the color of the LED, keeping it the same even after a power cycle. To use this feature, simply set the LED to your preferred color, and the program will automatically save the color to the board's flash memory. The saved color will be applied the next time you power on the board.

## Credits

This program was developed as part of the Embedded Systems Laboratory final assignment by [Elyorbek Kholikov]. The program is licensed under the [MIT licence]. Contributions are welcome!
