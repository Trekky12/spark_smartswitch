// Install the LowPower library for optional sleeping support.
// See loop() function comments for details on usage.
//#include <LowPower.h>

#include "WS2812B.h"
#include "MCP23017.h"
#include "SparkIntervalTimer.h"
#include "QueueList.h"
#include "Sonos.h"


// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 for Arduino and D1 for Spark Core (i2c clock) 
// Connect pin #13 of the expander to Analog 4 for Arduino and D0 for Spark Core (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Output #0 is on pin 21 so connect an LED or whatever from that to ground

// Insert firearm metaphor here
/* The Spark Core's manual mode puts everything in your hands. 
This mode gives you a lot of rope to hang yourself with, so tread cautiously. */
SYSTEM_MODE(SEMI_AUTOMATIC);

//WS2812B leds;

Adafruit_MCP23017 mcp;

IntervalTimer myTimer;

SONOSClient mySonos;

unsigned long lastSent = 0;
bool currentMute = FALSE;

int changeCount =0;




// DEFINES for selective debugging (most done via Serial console)
// #define INT_DEBUG 1
// #define QUEUE_DEBUG 1
// #define SERIAL_DEBUG

#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_T_HOLD 1000
#define BTN_T_DOUBLE 200
#define BTN_COUNT 6
#define MAX_QUEUE_LENGTH 3



// millis of the last btn up
unsigned long btn_last_up   [BTN_COUNT] = {0UL};

// millis of the last btn down
unsigned long btn_last_down [BTN_COUNT] = {0UL};

// Interrupts from the MCP will be handled by this PIN
byte SparkIntPIN = D3;

volatile int pressCount = 0;

int LED = D7;

volatile boolean connectToCloud = false;

volatile boolean inInterrupt = false;


//flag telling main loop to report on interrupt
volatile boolean serviceint = false;




// Define eventtypes and buttons
// btn can be one specific button or a sequence of buttons

enum btn {
    BTN_1 = 1, BTN_2 = 2, BTN_3 = 3, BTN_4 = 4, BTN_5 = 5, BTN_6 = 6, BTN_SEQUENCE = 7
};

// btn_event_type can be based one one button or a specific sequence
enum btn_event_type {
    BTN_SINGLE, BTN_DOUBLE, BTN_TRIPLE, BTN_HOLD, SEQUENCE_1, SEQUENCE_2
};


typedef struct {
    uint8_t btn;
    uint8_t event;
} t_btn_event;




// create a queue of t_btn_events.
QueueList <t_btn_event> btn_event_queue;


// connect to the cloud

void connect() {
    if (Spark.connected() == false) {
        Spark.connect();
    }
}





void handleButtonINT() {
    inInterrupt = true;
    
    uint8_t pin=mcp.getLastInterruptPin();
    uint8_t val=mcp.getLastInterruptPinValue();
    
    
    //NOTE: These next 2 will clear the interrupt.  The problem is that if the user
    //has _not_ released the button yet, it will simply re-interrupt again.  We
    //were seeing this every 30-60ms (about the time it takes for the user to
    //depress the button.  The solution is to put in a DELAY here.  This way,
    //if the user holds down the button it can continue to interrupt (as they
    //may intend) but at a reasonable rate.
    //I found a quick depress to be 30ms and a slow depress to be about 70ms.   
    
    //intcapAB = mcp.getInterruptCaptureAB();    //this will clear the MCP interrupt
    //valAB = mcp.readGPIOAB();                  //this will clear the MCP interrupt
    mcp.readGPIOAB();
    
    // relevant PIN
    if (pin >= 1 && pin <=16) 
    {
      // current time, so we don't have to call millis() all the time
  	  unsigned long now = millis();
      // index in array starts at zero
      int idx = pin - 1;
    
      // button release
      if(val == BTN_UP) 
      {
          // hold condition
          if (now-btn_last_down[idx] >= BTN_T_HOLD)
          {
            t_btn_event _btn_event;
            _btn_event.btn = pin;
            _btn_event.event = BTN_HOLD;
            btn_event_queue.push(_btn_event);
          }
        	// double click condition
        	else if ((now-btn_last_up[idx]<BTN_T_DOUBLE) && (now-btn_last_down[idx] < BTN_T_HOLD)) {
        	  t_btn_event _btn_event;
        	  _btn_event.btn = pin;
        	  _btn_event.event = BTN_DOUBLE;
        	  btn_event_queue.push(_btn_event);
        	} 
          // otherwise we have seen a single click
          else 
          {
            t_btn_event _btn_event;
            _btn_event.btn = pin;
            _btn_event.event = BTN_SINGLE;
            btn_event_queue.push(_btn_event);
          }
          btn_last_up[idx] = millis();
      }
      // button press	
      else if (val == BTN_DOWN) 
      {
          btn_last_down[idx] = millis();
      } 
      // unrecognized button event
      else 
      {
        //DEBUG
      }
	  }
    serviceint = true;
    pressCount = pressCount + 1;
    inInterrupt = false;
}








void setup() {

  delay(1000);

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    
    while(!Serial.available()) {  // Wait here until the user presses ENTER 
          SPARK_WLAN_Loop();        // in the Serial Terminal. Call the BG Tasks
    }
#endif /* SERIAL_DEBUG */
  
  
  
     WiFi.connect();

     while (!WiFi.ready()) SPARK_WLAN_Loop();


    pinMode(SparkIntPIN, INPUT);

    // leds.setup(4);
    // leds.setColor(0, 255, 255, 0);
    // leds.setColor(1, 0, 255, 0);
    // leds.setColor(2, 0, 0, 255);
    // leds.setColor(3, 255, 0, 0);
    // leds.show();


    mcp.begin(); // use default address 0


    mcp.pinMode(BTN_1, INPUT);
    mcp.pullUp(BTN_1, HIGH);

    mcp.pinMode(BTN_2, INPUT);
    mcp.pullUp(BTN_2, HIGH);


    pinMode(LED, OUTPUT);

    // Spark Interupt 
    attachInterrupt(SparkIntPIN, handleButtonINT, FALLING);

    // AUTO allocate printQcount to run every 1000ms (2000 * .5ms period)
    // myTimer.begin(printQcount, 3000, hmSec);

    mcp.setupInterrupts(true, false, LOW);

    mcp.setupInterruptPin(BTN_1, CHANGE);
    mcp.setupInterruptPin(BTN_2, CHANGE);

    mcp.readGPIOAB();

    inInterrupt = true;
}

/**
 * main routine: sleep the arduino, and wake up on Interrups.
 * the LowPower library, or similar is required for sleeping, but sleep is simulated here.
 * It is actually posible to get the MCP to draw only 1uA while in standby as the datasheet claims,
 * however there is no stadndby mode. Its all down to seting up each pin in a way that current does not flow.
 * and you can wait for interrupts while waiting.
 */
void loop() {

    if (connectToCloud) {
        connect();
        #ifdef SERIAL_DEBUG
        Serial.println("Connected to the cloud");
        #endif /* SERIAL DEBUG */
        connectToCloud = false;
    }

    #ifdef SERIAL_DEBUG
    if (pressCount > 50) {
        Serial.println("50 interrupts...");
        pressCount = 0;
    }
    #endif /* SERIAL DEBUG */

    #ifdef SERIAL_DEBUG
    if (serviceint) {
        Serial.println("=============================");
        Serial.println("Interrupt detected!");
        Serial.println(millis());
        serviceint = false;
    }
    #endif /* SERIAL DEBUG */


   if (!btn_event_queue.isEmpty()) {
      t_btn_event _btn_event = btn_event_queue.pop();

      if (_btn_event.event == BTN_SINGLE) {
        // warten double click time
        delay(BTN_T_DOUBLE);
        // we waited for some time, is there something new in the queue?
        if(!btn_event_queue.isEmpty()) {
          t_btn_event _btn_next_event = btn_event_queue.peek();
          // does the new event concern the same button as the current ?
          // is is the old current event a single click ? This is necessairy so we
          // don't throw away two following double clicks or something like that
            if(_btn_event.btn == _btn_next_event.btn && _btn_event.event == BTN_SINGLE) {
              // we assume the current event was only the first part of a double click
              // so we skip it and replace it with the new one
              // the new one will be popped from the queue
              _btn_event = btn_event_queue.pop();
            }
        }
      }

      // interpret the event and fire desired action
      // TODO shall be sourced out
      switch(_btn_event.btn) {
        case BTN_1:
          switch (_btn_event.event) {
            case BTN_SINGLE:
              mySonos.mute(TRUE);
              break;

            case BTN_DOUBLE:
              mySonos.mute(TRUE);
              break;

            case BTN_HOLD:
              mySonos.mute(TRUE);
              break;

            default:
              break;
          }
          break;
        case BTN_2:
          switch (_btn_event.event) {
            case BTN_SINGLE:
              mySonos.mute(FALSE);
              break;

            case BTN_DOUBLE:
            mySonos.mute(TRUE);
              break;

            case BTN_HOLD:
              //get the spark back to the cloud
              connectToCloud = true;
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
  }
