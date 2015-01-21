#include "SmartSwitchConfig.h"
// Dynamicly added includes
#include "clients/HTTPGETClient/HTTPGETClient.h"



// Device instantiation
HTTPGETClient raspberry_HTTPGETClient;



SMARTSWITCHConfig::SMARTSWITCHConfig() {
}



void SMARTSWITCHConfig::setup() {
// initialize what is needed
// basically we call setIP() for each client device
raspberry_HTTPGETClient.setIP(192, 168, 1, 103);
raspberry_HTTPGETClient.setPort(80);
}



// interpret the event and fire desired action
void SMARTSWITCHConfig::process(t_btn_event* e) {
switch(e->btn) {
   case BTN_0:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
       break;
     case BTN_DOUBLE:
       
       break;
     }
     break;
   case BTN_1:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
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
       
       break;
     case BTN_DOUBLE:
       
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
       raspberry_HTTPGETClient.sendRequest("/wecker/lib/powerpi.php?action=setsocket&socket=Bett%20Lampe&status=0", "");
       break;
     case BTN_DOUBLE:
       raspberry_HTTPGETClient.sendRequest("/wecker/lib/powerpi.php?action=setsocket&socket=Bett%20Lampe&status=1", "");
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
   case BTN_7:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       raspberry_HTTPGETClient.sendRequest("/wecker/lib/powerpi.php?action=setsocket&socket=Bett%20Lampe&status=0", "");
       break;
     case BTN_DOUBLE:
       raspberry_HTTPGETClient.sendRequest("/wecker/lib/powerpi.php?action=setsocket&socket=Bett%20Lampe&status=1", "");
       break;
     }
     break;
}
}
