// // // // // // // // // // // // // // // // // // // // //
//
// Includes
//

// hardware libraries
#include "MCP23017.h"
#include "neopixel.h"

// software libraries
#include "SmartSwitchConfig.h"
//#include "SparkIntervalTimer.h"
#include "QueueList.h"

// Insert firearm metaphor here
/* The Spark Core's manual mode puts everything in your hands.
This mode gives you a lot of rope to hang yourself with, so tread cautiously. */
//SYSTEM_MODE(SEMI_AUTOMATIC);

#define PIXEL_PIN A5
#define PIXEL_COUNT 12
#define PIXEL_TYPE WS2812
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// // // // // // // // // // // // // // // // // // // // //
//
// Global instances
//

Adafruit_MCP23017 mcp;
//IntervalTimer myTimer;
SMARTSWITCHConfig myConfig;


// // // // // // // // // // // // // // // // // // // // //
//
// Defines and constants
//

#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_T_HOLD 1000
#define BTN_T_DOUBLE 250
#define BTN_T_HOLD_DEBOUNCE 250
#define DEBOUNCE_DELAY 20

/* Disable RGB LED*/
#define DISABLE_RGB

// at the moment our doubleWait is uint8_t
// if you want to have more buttons you have
// to change the size of this bitfield
// only increasing BTN_COUNT will lead to unexpected
// behaviour and or runtime crashes
#define BTN_COUNT 8

#define MAX_QUEUE_LENGTH 3

// MACROS for the doubleWait bitfield
#define waitFor(btn,var) var |= (1<<btn)
#define dontWaitFor(btn,var) var &= ~(1<<btn)
#define isWaitingFor(btn,var) var >> btn & 1



// // // // // // // // // // // // // // // // // // // // //
//
// Globals
//

// millis of the last btn up
unsigned long btn_last_up [BTN_COUNT] = {0UL};

// millis of the last btn down
unsigned long btn_last_down [BTN_COUNT] = {0UL};

// millis of the last btn hold
unsigned long btn_last_hold [BTN_COUNT] = {0UL};

// Debounce
unsigned long last_interrupt = 0UL;

// Interrupts from the MCP will be handled by this PIN
byte SparkIntPIN = D3;

volatile boolean inInterrupt = false;

// bitfield to keep track of double clicks
volatile uint8_t doubleWait = 0;

// create a queue of t_btn_events.
QueueList <t_btn_event> btn_event_queue;

#define LED_TIMEOUT 2000
#define LED_BTN_TIMEOUT 200

// shutdown leds after 10 sec
volatile int lastLedAction = 0;

// This function gets called whenever there is a matching API request
// the command string format is
// <Brightness>,<LED_Red>,<LED_Green>,<LED_Blue>
// for example: 255,000,255,000,1
int ledControlAllRGB(String command) {

    int red = 0;
    int blue = 0;
    int green = 0;
    int brightness = 0;
    int autooff = 0;

    char * params = new char[command.length() + 1];

    strcpy(params, command.c_str());
    char * param1 = strtok(params, ",");
    char * param2 = strtok(NULL, ",");
    char * param3 = strtok(NULL, ",");
    char * param4 = strtok(NULL, ",");


    if (param1 != NULL && param2 != NULL && param3 != NULL && param4 != NULL) {

        brightness = atoi(param1);
        red = atoi(param2);
        green = atoi(param3);
        blue = atoi(param4);

        if (brightness < 0 || brightness > 255) return -1;
        if (red < 0 || red > 255) return -1;
        if (green < 0 || green > 255) return -1;
        if (blue < 0 || blue > 255) return -1;

        strip.setBrightness(brightness);
        strip.colorAll(strip.Color(red,green,blue));
        lastLedAction = millis();
        return 0;
    }
    return -1;
}


void handleButtonINT() {
    noInterrupts();
    inInterrupt = true;
}

void pushEvent(uint8_t btn, uint8_t event) {

    // we do not process events while the queue is full
    if (btn_event_queue.count() < MAX_QUEUE_LENGTH) {
        t_btn_event _btn_event;
        _btn_event.btn = btn;
        _btn_event.event = event;
        btn_event_queue.push(_btn_event);
    }

    // if we detected a button single event, we can stop wating for
    // a double click to happen on this pin
    if (event == BTN_SINGLE)
        dontWaitFor(btn, doubleWait);
}

void processSingleClicks() {
    unsigned short currBtn = 0;
    unsigned long now = millis();

    for (currBtn = 0; currBtn < BTN_COUNT; currBtn++) {
        if (isWaitingFor(currBtn, doubleWait)) {
            // time since btn_last_up is bigger than BTN_T_DOUBLE
            // and button is already released (last up is bigger than last down)
            if ((now - btn_last_up[currBtn]) >= BTN_T_DOUBLE) {
                if (btn_last_up[currBtn] < btn_last_down[currBtn]) {
                    if (btn_last_hold[currBtn] < btn_last_up[currBtn]) {
                        pushEvent(currBtn, BTN_SINGLE);
                    }
                } else {
                    // explicitly check if button is really pressed,
                    // otherwise: if we miss the btn_up we loop here forever...
                    if (now - btn_last_hold[currBtn] >= BTN_T_HOLD_DEBOUNCE && mcp.digitalRead(currBtn) == 0) {
                        if (btn_last_hold[currBtn] <= btn_last_down[currBtn]) {
                            pushEvent(currBtn, BTN_HOLD_CLICK);
                        }

                        pushEvent(currBtn, BTN_HOLD);
                        btn_last_hold[currBtn] = now;
                    }
                }
            }
        }
    }
}

void processButtonINT() {

    // only read the mcp if we are in the interrupt
    // if we are called from doubleWait, just jump to click interpretation

    if (millis() - last_interrupt > DEBOUNCE_DELAY) {

        uint8_t pin = mcp.getLastInterruptPin();
        uint8_t val = mcp.getLastInterruptPinValue();

        if (pin >= 0 && pin < BTN_COUNT) {

            // current time, so we don't have to call millis() all the time
            unsigned long now = millis();

            // button release
            if (val == BTN_UP) {
                if ((millis() - btn_last_up[pin]) < BTN_T_DOUBLE) {

                    pushEvent(pin, BTN_DOUBLE);
                    dontWaitFor(pin, doubleWait);

                } else {
                    // last up is farer in the past than BTN_T_DOUBLE,
                    // this might become a double click, but we don't know
                    // so we just wait
                    waitFor(pin, doubleWait);
                }
                btn_last_up[pin] = now;

            } else if (val == BTN_DOWN) {
                btn_last_down[pin] = now;

            } else {
                // unrecognized button event
                //DEBUG unexpexted VALs
            }
        }
    }

    //this will clear the MCP interrupt
    mcp.readGPIOAB();

}

void setup() {

    delay(1000);

    // connect to the WiFi
    WiFi.connect();
    // wait until it is actually connected
    while (!WiFi.ready()) Spark.process();

    // initialize configuration
    myConfig.setup();

    //Register our Spark function here
    Spark.function("ledrgball", ledControlAllRGB);

    pinMode(SparkIntPIN, INPUT);


    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    lastLedAction = millis();


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

    mcp.pinMode(BTN_6, INPUT);
    mcp.pullUp(BTN_6, HIGH);

    mcp.pinMode(BTN_7, INPUT);
    mcp.pullUp(BTN_7, HIGH);


    // AUTO allocate printQcount to run every 1000ms (2000 * .5ms period)
    // myTimer.begin(printQcount, 3000, hmSec);


    // first boolean expression mirros interrupts of the two banks of the MCP
    // it seems not to interfere with our use case, but we don't need it
    // so it is set to false
    mcp.setupInterrupts(false, false, LOW);

    //   TODO:
    //      source out to SMARTSWITCHConfig.setup() ?

    mcp.setupInterruptPin(BTN_0, CHANGE);
    mcp.setupInterruptPin(BTN_1, CHANGE);
    mcp.setupInterruptPin(BTN_2, CHANGE);
    mcp.setupInterruptPin(BTN_3, CHANGE);
    mcp.setupInterruptPin(BTN_4, CHANGE);
    mcp.setupInterruptPin(BTN_5, CHANGE);
    mcp.setupInterruptPin(BTN_6, CHANGE);
    mcp.setupInterruptPin(BTN_7, CHANGE);

    mcp.readGPIOAB();

    // Spark Interupt
    attachInterrupt(SparkIntPIN, handleButtonINT, FALLING);

}

/**
 * main routine
 */
void loop() {

    if (inInterrupt) {
        inInterrupt = false;
        interrupts();
        processButtonINT();
        last_interrupt = millis();
    }

    // check if there are clicks to process
    if (doubleWait > 0) {
        processSingleClicks();
    }

    /* stop LEDs after LED_TIMEOUT seconds */
    if (lastLedAction != -1 && (lastLedAction > 0 && millis() - lastLedAction > LED_TIMEOUT)) {
        strip.colorAll(strip.Color(0, 0, 0));
    }

#ifdef DISABLE_RGB
        RGB.control(true);
        RGB.brightness(0);
#endif /* DISABLE_RGB */


    if (!btn_event_queue.isEmpty()) {
        t_btn_event _btn_event = btn_event_queue.pop();
        // interpret the event and fire desired action
       myConfig.process(&_btn_event, &strip);
       lastLedAction = (millis() - LED_TIMEOUT) + LED_BTN_TIMEOUT;
    }

}
