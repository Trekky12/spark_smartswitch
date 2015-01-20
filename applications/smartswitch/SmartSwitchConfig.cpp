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
       wohnzimmer_SONOSClient.setMute(TRUE);
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
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     }
     break;
   case BTN_2:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     }
     break;
   case BTN_3:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     }
     break;
   case BTN_4:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     }
     break;
   case BTN_5:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.toggleMute();
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     }
     break;
}
}
