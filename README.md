# obs-esp8266-tallylight
Build small WIFI enabled tallylights that work with the [obs-websocket](https://github.com/Palakis/obs-websocket) plugin.
The tallylight looks for a substring in the current scene name and when it matches it will light up red. Also works with the preview in studio mode (green light).

## Hardware
I used the esp01 module, but other esp modules should work too. You can build the circuit on perfboard, but if you want a clean and small package you can manufacture a pcb with the gerberfiles.

### Assembly
Use the partslist.txt and schematic.png for reference. I find it the easiest to start with the smd components. First I pre tin the pads and then carefully solder on the USB connector. Since we only need power, you only need to solder on the two outer pins. Be carefull that you don't make any shotcircuits. Next solder on the LEDs, again be careful not to cause any shortcircuits. After that check with a multi meter for shortcircuits. Now you can solder on the rest of the components.

## Software
The Programm is far from finished, my C/C++ skills are a little rusty :). Right now you need to specify the fixed IP adress of your obs machine and authentication isn't working.

### OBS Setup
Install the [obs-websocket](https://github.com/Palakis/obs-websocket) plugin.
I recommend that you set the IP of your pc that is running OBS to a static one. Otherwise you have to reflash the esp every time your pc gets a different ip from DHCP.
Important: Don't activate athentication the tallylights currently don't support it!

### ESP8266 Setup
You need to setup the Arduino IDE with ESP8266 support. Check the [esp8266/Arduino](https://github.com/esp8266/Arduino) repo for details.
You'll also need the [ESP8266-Websocket](https://github.com/morrissinger/ESP8266-Websocket) and [ArduinoJson](https://arduinojson.org) librarys.

Next you need to edit the #defines to configure the program:
| Name | Description |
| ----------- | ----------- |
| WIFI_SSID | Your WIFI SSID |
| WIFI_PASS | The passfrase of your WIFI |
| OBS_WEBSOCKET_IP | Here you need to enter the IP of the computer that obs is running on |
| OBS_WEBSOCKET_PORT | Here you can specify the port. 4444 is the obs-websocket default port |
| OBS_WEBSOCKET_PASS | This is currently not working |
| OBS_SCENE_NAME | Here you need to enter the Substring that the tallylight should listen for. As an Example "Cam 1"|

Now you can flash the programm onto the esp and it should connect to your WIFI and OBS instance.

### Errors
I added some error codes:
| Name | Description |
| ----------- | ----------- |
| Blue constant flashing | The tallylight tries to connect to your WIFI |
| Blue burst flashing | The tallylight is connected to your WIFI but can't connect to OBS |
| Orange | Part of the disabled authentication, should not happen :D |
