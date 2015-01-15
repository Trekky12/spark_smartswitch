#include "SmartSwitchConfig.h"
// Dynamicly added includes
#include "SONOSClient.h"



// Device instantiation
SONOSClient wohnzimmer_SONOSClient;



SMARTSWITCHConfig::SMARTSWITCHConfig()
{
}



void SMARTSWITCHConfig::setup()
{
// initialize what is needed
//TODO mySonos.setIP("");
}



void SMARTSWITCHConfig::process(t_btn_event* e)
{
 // initialize what is needed
  // interpret the event and fire desired action
switch(e->btn) {
   case BTN_1:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.setMute(TRUE);
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
   case BTN_2:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       wohnzimmer_SONOSClient.setMute(FALSE);
       break;
     case BTN_DOUBLE:
       wohnzimmer_SONOSClient.setMute(FALSE);
       break;
     }
     break;
   case BTN_3:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
   case BTN_4:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
   case BTN_5:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
   case BTN_6:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
}
}
