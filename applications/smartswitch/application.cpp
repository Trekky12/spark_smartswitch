// Install the LowPower library for optional sleeping support.
// See loop() function comments for details on usage.
//#include <LowPower.h>

#include "MCP23017.h"
#include "SparkIntervalTimer.h"
#include "QueueList.h"
//#include "WS2812B.h"

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


Adafruit_MCP23017 mcp;
//WS2812B leds;

IntervalTimer myTimer;




// DEFINES for selective debugging (most done via Serial console)
// #define INT_DEBUG 1
// #define QUEUE_DEBUG 1

#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_T_HOLD 1000
#define BTN_T_DOUBLE 200

// millis of the last btn up
unsigned long btn_last_up   = 0UL;

// millis of the last btn down
unsigned long btn_last_down = 0UL;


// Interrupts from the MCP will be handled by this PIN
byte SparkIntPIN=D3;

volatile int pressCount = 0;

int LED = D7;

volatile boolean connectToCloud = false;

volatile boolean inInterrupt = false ;


//flag telling main loop to report on interrupt
volatile boolean serviceint = false;        


// message structure for events (button presses)
typedef struct {         /* deklariert den Strukturtyp person */
  uint8_t       btn_id;
  uint8_t       btn_val;
  unsigned long btn_time;
} t_interrupt_event;

// Define eventtypes and buttons

// btn can be one specific button or a sequence of buttons
enum btn  {BTN_1=1, BTN_2=2, BTN_3=3, BTN_4=4, BTN_5=5, BTN_6=6, BTN_SEQUENCE=7};

// btn_event_type can be based one one button or a specific sequence
enum btn_event_type {BTN_SINGLE, BTN_DOUBLE, BTN_TRIPLE, SEQUENCE_1, SEQUENCE_2};



typedef struct {
  uint8_t  btn;
  uint8_t  event;
} t_btn_event;


// create a queue of t_interrupt_events.
QueueList <t_interrupt_event> interrupt_queue;


// create a queue of t_btn_events.
QueueList <t_btn_event> btn_event_queue;


// connect to the cloud
void connect() {
  if (Spark.connected() == false) {
    Spark.connect();
  }
}




// void printQcount() {
//
//   Serial.print("Current INTERRUPT_QUEUE_LEN: ");
//   Serial.println(interrupt_queue.count());
//
// }



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
        
        // TODO:  catch cases when interrupt happended but no button was pressed
        //        do they exist? how can we recognize them? 
        //        255 as value ? no "lastInterruptPin" ?
      

          /*t_interrupt_event _event;
          
          // copy values to the struct
          _event.btn_id  = pin;
          _event.btn_val = val;
          _event.btn_time = millis();
      
          // push it to the interrupt_queue
          interrupt_queue.push(_event);
          */
          // get the spark back to the cloud
          if(pin == BTN_1) {
            Serial.println("Connecting to the cloud");
            connectToCloud = true;
          }

	// relevant PIN
        if (pin >= 1 && pin <=16) {
	  unsigned long now = millis();


           
	  // button release
	  if(val == BTN_UP) {
      
      // Serial.print("now-up: ");
      // Serial.println((now-btn_last_up));
      //
      // Serial.print("now-down: ");
      // Serial.println((now-btn_last_down));

  		// double click bedingung
  	if ((now-btn_last_up<BTN_T_DOUBLE) && (now-btn_last_down < BTN_T_HOLD)) {
  	 	       t_btn_event _btn_event;
  	         _btn_event.btn = pin;
  	       	 _btn_event.event = BTN_DOUBLE;
  	   		   btn_event_queue.push(_btn_event);
  	} else {
            t_btn_event _btn_event;
            _btn_event.btn = pin;
            _btn_event.event = BTN_SINGLE;
            btn_event_queue.push(_btn_event);
    }


	     btn_last_up = millis();
          } 
	  
	  // button press	
	  else if (val == BTN_DOWN) 
	  {
      	  	btn_last_down = millis();
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


void setup(){

  delay(2000);

  Serial.begin(9600);

  delay(1000);


//  while(!Serial.available())  // Wait here until the user presses ENTER 
//    SPARK_WLAN_Loop();        // in the Serial Terminal. Call the BG Tasks
                              // while we are hanging around doing nothing.
  
  
  pinMode(SparkIntPIN, INPUT);
  
  mcp.begin();      // use default address 0
  
 
  mcp.pinMode(BTN_1, INPUT);
  mcp.pullUp(BTN_1, HIGH); 
  
  mcp.pinMode(BTN_2, INPUT);
  mcp.pullUp(BTN_2, HIGH); 
  
  
  pinMode(LED, OUTPUT);
  
  // Spark Interupt 
  attachInterrupt(SparkIntPIN,handleButtonINT,FALLING);
  
  // AUTO allocate printQcount to run every 1000ms (2000 * .5ms period)
  // myTimer.begin(printQcount, 3000, hmSec);
  
  mcp.setupInterrupts(true,false,LOW);
  
  mcp.setupInterruptPin(BTN_1,CHANGE);
  mcp.setupInterruptPin(BTN_2,CHANGE);
  
  Serial.println("MCP23017 Interrupt Test");
  
  mcp.readGPIOAB();
  
  inInterrupt = true;


  /*leds.setup(4);
  leds.setColor(0, 255, 255, 0);
  leds.setColor(1, 0, 255, 0);	 
  leds.setColor(2, 0, 0, 255);
  leds.setColor(3, 255, 0, 0);
  leds.show();*/

}

/**
 * main routine: sleep the arduino, and wake up on Interrups.
 * the LowPower library, or similar is required for sleeping, but sleep is simulated here.
 * It is actually posible to get the MCP to draw only 1uA while in standby as the datasheet claims,
 * however there is no stadndby mode. Its all down to seting up each pin in a way that current does not flow.
 * and you can wait for interrupts while waiting.
 */
void loop(){
  
    if (connectToCloud) {
      connect();
      Serial.println("Connected to the cloud");
      connectToCloud = false;
    }
  
    if (pressCount > 50) {
      Serial.println("50 interrupts...");
      pressCount = 0;
    }
    
    if(serviceint) {
#ifdef INT_DEBUG
    Serial.println("=============================");
    Serial.println("Interrupt detected!");
    Serial.println(millis());
#endif /* INT_DEBUG */
    serviceint = false;
    }
    
    
    // if(!inInterrupt) {
    //       // single press
    //       if ((now-btn_last_up>BTN_T_DOUBLE) && (now-btn_last_down < BTN_T_HOLD)) {
    //   //else {
    //            t_btn_event _btn_event;
    //        _btn_event.btn = pin;
    //        _btn_event.event = BTN_SINGLE;
    //             btn_event_queue.push(_btn_event);
    //       }
    // }

    

    if (!btn_event_queue.isEmpty()) {
      t_btn_event _btn_event = btn_event_queue.pop();
      
      if (_btn_event.event == BTN_SINGLE) {

        // warten double click time
        delay(BTN_T_DOUBLE);
        
        // nach warten was neues in der queue ?
        if(!btn_event_queue.isEmpty()) {
          t_btn_event _btn_next_event = btn_event_queue.peek();

          // nächstes Event für gleichen button ?
            if(_btn_event.btn == _btn_next_event.btn) {
              _btn_event = btn_event_queue.pop();
            }
        }
      }
      
      //TODO queue.peek() 
      
      
      if (_btn_event.event == BTN_SINGLE)
      {
        digitalWrite(LED, HIGH);

      }
      
      if (_btn_event.event == BTN_DOUBLE)
      {
        digitalWrite(LED, LOW);
 
      }
      
      
      
      //Serial.print("Event type: ");
      //Serial.println(_btn_event.event);
  
  
  
        //
        // #ifdef QUEUE_DEBUG
        //       char debug_buff[64];
        //       sprintf(debug_buff, "POPPED BTN Q - BTN: %d EVENT: %d", _btn_event.btn, _btn_event.event);
        //       Serial.println(debug_buff);
        // #endif /* QUEUE_DEBUG */
        //
        //       if (_btn_event.event == BTN_SINGLE) {
        //         switch (_btn_event.btn) {
        //           case BTN_1:
        //           Serial.println("BTN1 SINGLE PRESS");
        //             break;
        //           case BTN_2:
        //           Serial.println("BTN2 SINGLE PRESS");
        //             break;
        //         }
        //       }
    }

}
