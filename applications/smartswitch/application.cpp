// define whetere we are a buttonPad or behind a gira
#define __buttonPad
//#define __gira

// plugins to include
#define __sonos
#define __hue
#define __raspberry

//#include "spark_wiring.h"

// hardware libraries
#include "lib/WS2812B.h"
#include "lib/MCP23017.h"

// software libraries
#include "SmartSwitchConfig.h"
#include "lib/SparkIntervalTimer.h"
#include "lib/QueueList.h"

// Insert firearm metaphor here
/* The Spark Core's manual mode puts everything in your hands. 
This mode gives you a lot of rope to hang yourself with, so tread cautiously. */
SYSTEM_MODE(SEMI_AUTOMATIC);

#ifdef __buttonPad
WS2812B leds;
#endif /* buttonPad */


// to debug via serial console uncomment the following line:
#define SERIAL_DEBUG

Adafruit_MCP23017 mcp;

IntervalTimer myTimer;

SMARTSWITCHConfig myConfig;

unsigned long lastSent = 0;

int changeCount = 0;


#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_T_HOLD 1000
#define BTN_T_DOUBLE 200
#define BTN_COUNT 6
#define MAX_QUEUE_LENGTH 3


// millis of the last btn up
unsigned long btn_last_up [BTN_COUNT] = {0UL};

// millis of the last btn down
unsigned long btn_last_down [BTN_COUNT] = {0UL};

// Interrupts from the MCP will be handled by this PIN
byte SparkIntPIN = D3;

volatile int pressCount = 0;

int LED = D7;

volatile boolean inInterrupt = false;

// create a queue of t_btn_events.
QueueList <t_btn_event> btn_event_queue;

void handleButtonINT() {

    noInterrupts();

    uint8_t pin = mcp.getLastInterruptPin();
    uint8_t val = mcp.getLastInterruptPinValue();

#ifdef SERIAL_DEBUG
    Serial.println("Interrupt detected!");
    Serial.println(pin);
    Serial.println(val);
#endif /* SERIAL_DEBUG */

    //this will clear the MCP interrupt
    mcp.readGPIOAB();

    // relevant PIN
    // TODO check if queue has enough free space
    if (pin >= 1 && pin <= 16) {
        // current time, so we don't have to call millis() all the time
        unsigned long now = millis();
        // index in array starts at zero
        int idx = pin - 1;

        // button release
        // we do not process events while the queue is full
        if (val == BTN_UP && (btn_event_queue.count() < MAX_QUEUE_LENGTH)) {
            // hold condition
            if (now - btn_last_down[idx] >= BTN_T_HOLD) {
                t_btn_event _btn_event;
                _btn_event.btn = pin;
                _btn_event.event = BTN_HOLD;
                btn_event_queue.push(_btn_event);
            }// double click condition
            else if ((now - btn_last_up[idx] < BTN_T_DOUBLE) && (now - btn_last_down[idx] < BTN_T_HOLD)) {
                t_btn_event _btn_event;
                _btn_event.btn = pin;
                _btn_event.event = BTN_DOUBLE;
                btn_event_queue.push(_btn_event);
            }// otherwise we have seen a single click
            else {
                t_btn_event _btn_event;
                _btn_event.btn = pin;
                _btn_event.event = BTN_SINGLE;
                btn_event_queue.push(_btn_event);
            }
            btn_last_up[idx] = millis();
        }// button press	
        else if (val == BTN_DOWN) {
            btn_last_down[idx] = millis();
        }// unrecognized button event
        else {
            //DEBUG
        }
    }
    pressCount = pressCount + 1;
    inInterrupt = false;

    // attachInterrupt(SparkIntPIN, handleInterrupt, FALLING);
    interrupts();

}

void setup() {

    delay(1000);

#ifdef SERIAL_DEBUG
    Serial.begin(9600);

    while (!Serial.available()) { // Wait here until the user presses ENTER 
        SPARK_WLAN_Loop(); // in the Serial Terminal. Call the BG Tasks
    }
#endif /* SERIAL_DEBUG */


    // connect to the WiFi
    WiFi.connect();
    // wait until it is actually connected
    while (!WiFi.ready()) SPARK_WLAN_Loop();

    // initialize configuration
    myConfig.setup();

    pinMode(SparkIntPIN, INPUT);

#ifdef __buttonPad
    leds.setup(4);
    leds.setColor(0, 255, 255, 0);
    leds.setColor(1, 0, 255, 0);
    leds.setColor(2, 0, 0, 255);
    leds.setColor(3, 255, 0, 0);
    leds.show();
#endif /* __buttonPad */

    mcp.begin(); // use default address 0

    mcp.pinMode(BTN_0, INPUT);
    mcp.pullUp(BTN_0, HIGH);
    
    mcp.pinMode(BTN_1, INPUT);
    mcp.pullUp(BTN_1, HIGH);

    mcp.pinMode(BTN_2, INPUT);
    mcp.pullUp(BTN_2, HIGH);
    
    mcp.pinMode(BTN_3, INPUT);
    mcp.pullUp(BTN_3, HIGH);
    
    mcp.pinMode(BTN_4, INPUT);
    mcp.pullUp(BTN_4, HIGH);
    
    mcp.pinMode(BTN_5, INPUT);
    mcp.pullUp(BTN_5, HIGH);

    pinMode(LED, OUTPUT);

    // Spark Interupt 
    attachInterrupt(SparkIntPIN, handleButtonINT, FALLING);

    // AUTO allocate printQcount to run every 1000ms (2000 * .5ms period)
    // myTimer.begin(printQcount, 3000, hmSec);

    mcp.setupInterrupts(true, false, LOW);
    // TODO only initialize interrupts needed ?
    //      source out to SMARTSWITCHConfig.setup() ?
    mcp.setupInterruptPin(BTN_0, CHANGE);
    mcp.setupInterruptPin(BTN_1, CHANGE);
    mcp.setupInterruptPin(BTN_2, CHANGE);
    mcp.setupInterruptPin(BTN_3, CHANGE);
    mcp.setupInterruptPin(BTN_4, CHANGE);
    mcp.setupInterruptPin(BTN_5, CHANGE);
    
    mcp.readGPIOAB();

    inInterrupt = true;
}

/**
 * main routine
 */
void loop() {

#ifdef SERIAL_DEBUG
    if (pressCount > 50) {
        Serial.println("50 interrupts...");
        pressCount = 0;
    }
#endif /* SERIAL DEBUG */

    if (!btn_event_queue.isEmpty()) {
        t_btn_event _btn_event = btn_event_queue.pop();

        if (_btn_event.event == BTN_SINGLE) {
            // warten double click time
            delay(BTN_T_DOUBLE);
            // we waited for some time, is there something new in the queue?
            if (!btn_event_queue.isEmpty()) {
                t_btn_event _btn_next_event = btn_event_queue.peek();
                // does the new event concern the same button as the current ?
                // is is the old current event a single click ? This is necessairy so we
                // don't throw away two following double clicks or something like that
                if (_btn_event.btn == _btn_next_event.btn && _btn_event.event == BTN_SINGLE) {
                    // we assume the current event was only the first part of a double click
                    // so we skip it and replace it with the new one
                    // the new one will be popped from the queue
                    _btn_event = btn_event_queue.pop();
                }
            }
        }

        // interpret the event and fire desired action
        myConfig.process(&_btn_event);
    }

}
