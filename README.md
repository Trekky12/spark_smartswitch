# SmartSwitch
The SmartSwitch is a input device for (local) network components realized with a [Spark.io Core ](https://www.spark.io/) application.


The switch can be used to submit HTTP requests on the local network. This makes it possible to control a variety of devices. Also we provide a library to control [SONOS](http://www.sonos.com/) speakers.

Also you can use the switch with [IFTTT](http://ifttt.com), refer to our wiki. This enables the switch to control lots of internet enabled devices, refer to IFTTT for a complete list of possibilities.

## Hardware
* [Spark.io Core ](https://www.spark.io/)
  * [Documentation](https://docs.spark.io)
* [MCP23017 SPI I/O Expander](http://www.microchip.com/wwwproducts/Devices.aspx?product=MCP23017)
  * [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf)
* Pushbuttons and LEDs
  * ButtonPad on the SmartSwitch PCB with WS2812B LEDs 
    * [WS2812B Datasheet] (http://www.adafruit.com/datasheets/WS2812B.pdf)
  * GIRA Buttons (http://katalog.gira.de/de_DE/datenblatt.html?id=571789)
  * own pushbuttons and LEDs

## Functionality
The Button are triggered with single click, double click or holding.

The LEDs can be controled by the [Spark.IO Cloud API](http://docs.spark.io/api/).

* WS2812B LEDs:

        curl https://api.spark.io/v1/devices/<device_id>/ledrgb -d access_token=<access_token> -d "args=<LEDNumber>,<RED Value>,<GREEN Value>,<BLUE Value>"
* normal LEDs: 

        curl https://api.spark.io/v1/devices/<device_id>/led -d access_token=<access_token> -d "args=<LEDNumber>,<HIGH or LOW>"


## Usage

For detailed information on how to setup and use it refer to our Wiki: [Smart Switch Wiki](https://github.com/phhe/spark_smartswitch/wiki)


## Software modules
Initial there are modules for the following network devices:
* SONOS
  * Mute Control
  * Volume Control
* Philips Hue
  * On/Off
  * Dimm
* RaspberryPi/local Webserver
  * send Request to server
  * read Request from server

### Create own Module
It is possible to create a own module to connect the Buttons to.


# SmartSwitch PCB
Eagle Files are available at [https://github.com/Trekky12/SmartSwitchPCB](https://github.com/Trekky12/SmartSwitchPCB)
![](https://raw.githubusercontent.com/Trekky12/SmartSwitchPCB/master/SmartSwitch.png)
