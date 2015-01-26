#include "SmartSwitchConfig.h"
// Dynamicly added includes
#include "clients/SONOSClient/SONOSClient.h"



// Device instantiation
SONOSClient wohnzimmer_SONOSClient;



SMARTSWITCHConfig::SMARTSWITCHConfig() {
}



void SMARTSWITCHConfig::setup() {
// initialize what is needed
// basically we call setIP() for each client device
wohnzimmer_SONOSClient.setIP(10, 0, 6, 114);
}



// interpret the event and fire desired action
void SMARTSWITCHConfig::process(t_btn_event* e) {
switch(e->btn) {
   case BTN_0:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_1:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_2:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       Spark.publish("btn_pressed", "2");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     }
     break;
   case BTN_3:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       Spark.publish("btn_pressed", "3");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     }
     break;
   case BTN_4:
   switch (e->event) {
     case BTN_HOLD:
       wohnzimmer_SONOSClient.changeVolume(-10);
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.changeVolume(-15);
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.changeVolume(-25);
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_5:
   switch (e->event) {
     case BTN_HOLD:
       wohnzimmer_SONOSClient.changeVolume(10);
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.changeVolume(15);
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.changeVolume(25);
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_6:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       Spark.publish("btn_pressed", "6");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_7:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       Spark.publish("btn_pressed", "7");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
}
}
