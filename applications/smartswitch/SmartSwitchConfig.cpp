// TODO: GENERATE THIS FILE FROM config.json

#include "SmartSwitchConfig.h"

//dynamic includes
#include "sonos.h"

//SONOSClient mySonos;


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
        case BTN_SINGLE:
          //mySonos.toggleMute();
          break;
        case BTN_DOUBLE:
          //mySonos.setMute(FALSE);
          break;
        case BTN_HOLD:
          //mySonos.setMute(TRUE);
          break;
        default:
          break;
      }
      break;
    case BTN_2:
      switch (e->event) {
        case BTN_SINGLE:
          //mySonos.changeVolume(10);
          break;
        case BTN_DOUBLE:
          //mySonos.changeVolume(-10);
          break;
        case BTN_HOLD:
              //get the spark back to the cloud
              // connect to the cloud
              if (Spark.connected() == false) {
                Spark.connect();
              }
              #ifdef SERIAL_DEBUG
              Serial.println("Connected to the cloud");
              #endif /* SERIAL DEBUG */
          break;
        default:
          break;
      }
      break;
    default:
      ;
      break;
  }
  
}

