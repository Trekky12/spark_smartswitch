# SmartSwitch
The SmartSwitch is a input device for (local) network components realized with a [Spark.io Core ](https://www.spark.io/)<sup>1</sup> application. For further details on this project refer to our wiki here on github: [SmartSwitch WIKI](https://github.com/phhe/spark_smartswitch/wiki)


The switch can be used to submit HTTP requests on the local network. This makes it possible to control a variety of devices. Also we provide a library to control [SONOS](http://www.sonos.com/) speakers<sup>1</sup>.

Also you can use the switch with [IFTTT](http://ifttt.com)<sup>1</sup>, refer to our wiki. This enables the switch to control lots of internet enabled devices, refer to IFTTT for a complete list of possibilities.

You can find our project on [hackster.io](http://www.hackster.io) at [www.hackster.io/hdm/smartswitch](http://www.hackster.io/hdm/smartswitch).

![smartswitch control flow](https://raw.githubusercontent.com/wiki/phhe/spark_smartswitch/images/control.png)

<sup>1</sup><small>we are not associated with Spark, SONOS or IFTTT in any form. We are just two students making their homes a bit smarter</small>

## Hardware

![smartswitch hardware versions](images/hardware.png)

* 1	×	[Spark.io Core ](https://www.spark.io/)
  * [Documentation](https://docs.spark.io)
* 1	×	[MCP23017 I2C I/0 Expander](http://www.microchip.com/wwwproducts/Devices.aspx?product=MCP23017)
  * [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf)
* 2 ×	4,7K Ohm Resistor
  * I2C Pullup on Spark
* For the SmartSwitch with the Button Pad
  * 1 ×	[Button Pad Silicon Rubber](https://www.sparkfun.com/products/7836)
  * 4 ×	WS2812B LEDs
    * [Datasheet] (http://www.adafruit.com/datasheets/WS2812B.pdf)
* Gira Buttons
  * [GIRA Buttons Panel](http://katalog.gira.de/de_DE/datenblatt.html?id=571789)
  * 6 ×	3K Ohm Resistor
    * for the LEDs                

## Functionality
The Button are triggered with single click, double click or holding.

The LEDs can be controled by the [Spark.IO Cloud API](http://docs.spark.io/api/).

* WS2812B LEDs:

        curl https://api.spark.io/v1/devices/<device_id>/ledrgb -d access_token=<access_token> -d "args=<LEDNumber>,<RED Value>,<GREEN Value>,<BLUE Value>"
        
        curl https://api.spark.io/v1/devices/<device_id>/ledrgball -d access_token=<access_token> -d "args=<LED1_Red>,<LED1_Green>,<LED1_Blue>,<LED2_Red>,<LED2_Green>,<LED2_Blue>,<LED3_Red>,<LED3_Green>,<LED3_Blue>,<LED4_Red>,<LED4_Green>,<LED4_Blue>"
        
* normal LEDs: 

        curl https://api.spark.io/v1/devices/<device_id>/led -d access_token=<access_token> -d "args=<LEDNumber>,<HIGH or LOW>"

## Software modules
Initial there are modules for the following network devices:
* SONOS
  * Mute Control
  * Volume Control
* RaspberryPi/local Webserver
  * send Request to server
  * read Request from server
 
In progress:
* Philips Hue
  * On/Off
  * Dimm  

### Create own Module
It is possible to create a own module to connect the Buttons to.


## [Configuration](https://github.com/phhe/spark_smartswitch/wiki/Configuration)

# SmartSwitch PCB
Eagle Files are available at [https://github.com/Trekky12/SmartSwitchPCB](https://github.com/Trekky12/SmartSwitchPCB)
![](https://raw.githubusercontent.com/Trekky12/SmartSwitchPCB/master/SmartSwitch.png)
