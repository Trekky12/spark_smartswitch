import json
from pprint import pprint


# load the config file
# relative path looks little weird, because it gets called from 'build' folder
with open("SmartSwitchConfig.json") as json_file:
    config_data = json.load(json_file)


# read devices and device setup data
devices = config_data["devices"]

print '#include "SmartSwitchConfig.h"'

   
print '// Dynamicly added includes'
for device in devices:
    # include needed headers
    print '#include "%s.h"' % (device.split("_")[1])
    
print '\n\n'


print '// Device instantiation'
for device in devices:
        print "%s %s;" % (device.split("_")[1], device)


print '\n\n'


print 'SMARTSWITCHConfig::SMARTSWITCHConfig() {'
print '}'

print '\n\n'


print 'void SMARTSWITCHConfig::setup() {'
print '// initialize what is needed'
print '// basically we call setIP() for each client device'
for device in devices:
    # include needed headers
    # check if an IP is given and call setIP() if so
    device_params = config_data["devices"][device]
    
    if "IP" in device_params.keys():
        ipfield = devices[device]["IP"].split(".")
        print "%s.setIP(%s, %s, %s, %s);" % (device, ipfield[0], ipfield[1] , ipfield[2] , ipfield[3] )
        
    if "Port" in device_params.keys():
        print "%s.setPort(%s);" % (device,  device_params["Port"])
        


print '}'

print '\n\n'


print '// interpret the event and fire desired action'
print 'void SMARTSWITCHConfig::process(t_btn_event* e, Adafruit_NeoPixel* strip) {'

# open the switch case for event-button
print  "switch(e->btn) {"

# iterate through all buttons
for btn in  config_data["btns"].keys():
    print 2*" ", "case %s:" % btn
    
    if "LED_FEEDBACK" in config_data["btns"][btn].keys():
        if config_data["btns"][btn]["LED_FEEDBACK"] != "":
            print 2*" ", "strip->colorAll(strip->Color%s);" % config_data["btns"][btn]["LED_FEEDBACK"]
    
    # open the switch case for event-types
    print 2*" ", "switch (e->event) {"
    
    # iterate through all configured event-types for the current button
    for event in config_data["btns"][btn].keys():
        if event.startswith( 'BTN' ):
            print 4*" ", "case %s:" % event
            print 6*" ", config_data["btns"][btn][event]
            print 6*" ", "break;"

    # close the switch case for event-types
    print  4*" ", "}"
    print  4*" ", "break;"


# close the switch case for event-button
print "}"


print '}'











  # switch(e->btn) {
  #   case BTN_1:
  #     switch (e->event) {
  #       case BTN_SINGLE:
  #         //mySonos.toggleMute();
  #         break;
  #       case BTN_DOUBLE:
  #         //mySonos.setMute(FALSE);
  #         break;
  #       case BTN_HOLD:
  #         //mySonos.setMute(TRUE);
  #         break;
  #       default:
  #         break;
  #     }
  #     break;
  #   case BTN_2:
  #     switch (e->event) {
  #       case BTN_SINGLE:
  #         //mySonos.changeVolume(10);
  #         break;
  #       case BTN_DOUBLE:
  #         //mySonos.changeVolume(-10);
  #         break;
  #       case BTN_HOLD:
  #             //get the spark back to the cloud
  #             // connect to the cloud
  #             if (Spark.connected() == false) {
  #               Spark.connect();
  #             }
  #             #ifdef SERIAL_DEBUG
  #             Serial.println("Connected to the cloud");
  #             #endif /* SERIAL DEBUG */
  #         break;
  #       default:
  #         break;
  #     }
  #     break;
  #   default:
  #     ;
  #     break;
  # }