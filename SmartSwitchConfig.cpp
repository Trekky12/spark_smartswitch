#include "SmartSwitchConfig.h"
// Dynamicly added includes
#include "HTTPGETClient.h"



// Device instantiation
HTTPGETClient test_HTTPGETClient;



SMARTSWITCHConfig::SMARTSWITCHConfig() {
}



void SMARTSWITCHConfig::setup() {
// initialize what is needed
// basically we call setIP() for each client device
test_HTTPGETClient.setIP(192, 168, 1, 103);
test_HTTPGETClient.setPort(80);
}



// interpret the event and fire desired action
void SMARTSWITCHConfig::process(t_btn_event* e, Adafruit_NeoPixel* strip) {
switch(e->btn) {
   case BTN_0:
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       
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
       
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_3:
   strip->colorAll(strip->Color(0,0,255));
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       Spark.publish("btn_pressed", "3");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_4:
   strip->colorAll(strip->Color(255,0,0));
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       test_HTTPGETClient.sendRequest("test1.php", "");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_5:
   strip->colorAll(strip->Color(255,255,255));
   switch (e->event) {
     case BTN_HOLD:
       
       break;
     case BTN_SINGLE:
       test_HTTPGETClient.sendRequest("test2.php", "");
       break;
     case BTN_DOUBLE:
       
       break;
     case BTN_HOLD_CLICK:
       
       break;
     }
     break;
   case BTN_6:
   strip->colorAll(strip->Color(255,255,0));
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
   strip->colorAll(strip->Color(0,255,255));
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
