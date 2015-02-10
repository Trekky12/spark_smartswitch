// define whetere we are a buttonPad or behind a gira
#define __buttonPad
//#define __gira

// // // // // // // // // // // // // // // // // // // // //
//
// Includes
//

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
//SYSTEM_MODE(SEMI_AUTOMATIC);

#ifdef __buttonPad
WS2812B leds;
#endif /* buttonPad */

// // // // // // // // // // // // // // // // // // // // //
//
// Defines for Debugging
//

// to debug via serial console uncomment the following line:
//#define SERIAL_DEBUG

// to make the spark wait for [ENTER]
// on the serial console after booting
// #define SERIAL_WAIT

// these defines can be used for finer granularity 
// of the debug output
//#define INTERRUPT_DEBUG 


// // // // // // // // // // // // // // // // // // // // //
//
// Global instances
//

Adafruit_MCP23017 mcp;
IntervalTimer myTimer;
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

/* Disable LED when there is no WiFi */
#define WLAN_DISABLE_RGB
#define BUTTONPAD_LED_FEEDBACK

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

#ifdef __buttonPad

#define LED_TIMEOUT 10000
#define LED_BTN_TIMEOUT 200

#define 
// shutdown leds after 10 sec
volatile int lastLedAction = 0;

void setLEDs(
        int red1, int green1, int blue1,
        int red2, int green2, int blue2,
        int red3, int green3, int blue3,
        int red4, int green4, int blue4) {

    leds.setColor(0, red1, green1, blue1);
    leds.setColor(1, red2, green2, blue2);
    leds.setColor(2, red3, green3, blue3);
    leds.setColor(3, red4, green4, blue4);
    leds.show();
    lastLedAction = 0;
}
#endif /* __buttonPad */


// This function gets called whenever there is a matching API request
// the command string format is <led number>,<state>
// for example: 1,HIGH or 1,LOW
//              2,HIGH or 2,LOW

#ifdef __gira

int ledControl(String command) {

    int state = 0;
    int mcpPin = -1;
    // conversion of ascii to integer
    int ledNumber = command.charAt(0) - '0';

    /* Check for a valid digital pin */
    if (ledNumber < 0 || ledNumber > 5) return -1;

    // parse the state from the given command
    if (command.substring(2, 5) == "LOW") state = 0;
    else if (command.substring(2, 6) == "HIGH") state = 1;
    else return -2;

    // write to the appropriate pin on the mcp
    switch (ledNumber) {
        case 0:
            mcpPin = BTN_LED_0;
            break;
        case 1:
            mcpPin = BTN_LED_1;
            break;
        case 2:
            mcpPin = BTN_LED_2;
            break;
        case 3:
            mcpPin = BTN_LED_3;
            break;
        case 4:
            mcpPin = BTN_LED_4;
            break;
        case 5:
            mcpPin = BTN_LED_5;
            break;
    }
    if (mcpPin >= 0) {

#ifdef SERIAL_DEBUG
        Serial.print("Setting LED ");
        Serial.print(mcpPin);
        Serial.print(" to: ");
        Serial.println(state);
#endif /* SERIAL_DEBUG */

        mcp.digitalWrite(mcpPin, state);
        return 0;
    }
    // failure
    return -1;
}
#endif /* __gira */


#ifdef __buttonPad
// This function gets called whenever there is a matching API request
// the command string format is <led number>,<Red>,<Green>,<Blue>
// for example: 1,000,000,000

int ledControlRGB(String command) {

    int ledNumber = -1;
    int red = 0;
    int blue = 0;
    int green = 0;

    char * params = new char[command.length() + 1];

    strcpy(params, command.c_str());
    char * param1 = strtok(params, ",");
    char * param2 = strtok(NULL, ",");
    char * param3 = strtok(NULL, ",");
    char * param4 = strtok(NULL, ",");


    if (param1 != NULL && param2 != NULL && param3 != NULL && param4 != NULL) {
        ledNumber = atoi(param1);

        /* Check for a valid digital pin */
        if (ledNumber < 0 || ledNumber > 4) return -1;

        red = atoi(param2);
        green = atoi(param3);
        blue = atoi(param4);

        if (red < 0 || red > 255) return -1;
        if (green < 0 || green > 255) return -1;
        if (blue < 0 || blue > 255) return -1;
        leds.setColor(ledNumber, red, green, blue);
        leds.show();

        lastLedAction = millis();
        return 0;
    }
    return -1;
}


// This function gets called whenever there is a matching API request
// the command string format is 
// <LED1_Red>,<LED1_Green>,<LED1_Blue>,
// <LED2_Red>,<LED2_Green>,<LED2_Blue>, 
// <LED3_Red>,<LED3_Green>,<LED3_Blue>, 
// <LED4_Red>,<LED4_Green>,<LED4_Blue>
// for example: 000,000,000,000,000,000,000,000,000,000,000,000

int ledControlAllRGB(String command) {

    int red1, red2, red3, red4 = 0;
    int blue1, blue2, blue3, blue4 = 0;
    int green1, green2, green3, green4 = 0;

    char * params = new char[command.length() + 1];

    strcpy(params, command.c_str());
    char * param1 = strtok(params, ",");
    char * param2 = strtok(NULL, ",");
    char * param3 = strtok(NULL, ",");
    char * param4 = strtok(NULL, ",");
    char * param5 = strtok(NULL, ",");
    char * param6 = strtok(NULL, ",");
    char * param7 = strtok(NULL, ",");
    char * param8 = strtok(NULL, ",");
    char * param9 = strtok(NULL, ",");
    char * param10 = strtok(NULL, ",");
    char * param11 = strtok(NULL, ",");
    char * param12 = strtok(NULL, ",");


    if (param1 != NULL && param2 != NULL && param3 != NULL && param4 != NULL
            && param5 != NULL && param6 != NULL && param7 != NULL && param8 != NULL
            && param9 != NULL && param10 != NULL && param11 != NULL && param12 != NULL) {

        red1 = atoi(param1);
        green1 = atoi(param2);
        blue1 = atoi(param3);

        red2 = atoi(param4);
        green2 = atoi(param5);
        blue2 = atoi(param6);

        red3 = atoi(param7);
        green3 = atoi(param8);
        blue3 = atoi(param9);

        red4 = atoi(param10);
        green4 = atoi(param11);
        blue4 = atoi(param12);

        if (red1 < 0 || red1 > 255) return -1;
        if (green1 < 0 || green1 > 255) return -1;
        if (blue1 < 0 || blue1 > 255) return -1;

        if (red2 < 0 || red2 > 255) return -1;
        if (green2 < 0 || green2 > 255) return -1;
        if (blue2 < 0 || blue2 > 255) return -1;

        if (red3 < 0 || red3 > 255) return -1;
        if (green3 < 0 || green3 > 255) return -1;
        if (blue3 < 0 || blue3 > 255) return -1;

        if (red4 < 0 || red4 > 255) return -1;
        if (green4 < 0 || green4 > 255) return -1;
        if (blue4 < 0 || blue4 > 255) return -1;

        leds.setColor(0, red1, green1, blue1);
        leds.setColor(1, red2, green2, blue2);
        leds.setColor(2, red3, green3, blue3);
        leds.setColor(3, red4, green4, blue4);
        leds.show();

        lastLedAction = millis();
        return 0;
    }
    return -1;
}
#endif /* __buttonPad */

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
#ifdef SERIAL_DEBUG
                        Serial.print("Single Click: ");
                        Serial.println(currBtn);
#endif /* SERIAL_DEBUG */
                    }
                } else {
                    // explicitly check if button is really pressed,
                    // otherwise: if we miss the btn_up we loop here forever...
                    if (now - btn_last_hold[currBtn] >= BTN_T_HOLD_DEBOUNCE && mcp.digitalRead(currBtn) == 0) {
                        if (btn_last_hold[currBtn] <= btn_last_down[currBtn]) {
                            pushEvent(currBtn, BTN_HOLD_CLICK);
#ifdef SERIAL_DEBUG
                            Serial.print("Hold Click: ");
                            Serial.println(currBtn);
#endif /* SERIAL_DEBUG */
                        }

                        pushEvent(currBtn, BTN_HOLD);
                        btn_last_hold[currBtn] = now;
#ifdef SERIAL_DEBUG
                        Serial.print("Hold: ");
                        Serial.println(currBtn);
                        Serial.println(mcp.digitalRead(currBtn));
                        Serial.println(now);
                        Serial.println(btn_last_hold[currBtn]);

#endif /* SERIAL_DEBUG */
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

#ifdef SERIAL_DEBUG
#ifdef INTERRUPT_DEBUG
        //Serial.println(millis());
        Serial.print("Processing button interrupt for: ");
        Serial.print("PIN: ");
        Serial.print(pin);
        Serial.print(" VAL: ");
        Serial.println(val);
#endif /* INTERRUPT_DEBUG */
#endif /* SERIAL_DEBUG */


        if (pin >= 0 && pin < BTN_COUNT) {

            // current time, so we don't have to call millis() all the time
            unsigned long now = millis();

            // button release
            if (val == BTN_UP) {
                if ((millis() - btn_last_up[pin]) < BTN_T_DOUBLE) {

                    pushEvent(pin, BTN_DOUBLE);
                    dontWaitFor(pin, doubleWait);

#ifdef SERIAL_DEBUG
                    Serial.print("Double: ");
                    Serial.println(pin);
#endif /* SERIAL_DEBUG */

                } else {
                    // last up is farer in the past than BTN_T_DOUBLE,
                    // this might become a double click, but we don't know
                    // so we just wait
#ifdef SERIAL_DEBUG
                    Serial.println("wait for pin");
#endif /* SERIAL_DEBUG */
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

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
#ifdef SERIAL_WAIT
    while (!Serial.available()) { // Wait here until the user presses ENTER 
        SPARK_WLAN_Loop(); // in the Serial Terminal. Call the BG Tasks
    }
#endif /* SERIAL_WAIT */
#endif /* SERIAL_DEBUG */

    // connect to the WiFi
    WiFi.connect();
    // wait until it is actually connected
    while (!WiFi.ready()) SPARK_WLAN_Loop();

    // initialize configuration
    myConfig.setup();

    //Register our Spark function here
#ifdef __gira
    Spark.function("led", ledControl);
#endif

#ifdef __buttonPad
    Spark.function("ledrgb", ledControlRGB);
    Spark.function("ledrgball", ledControlAllRGB);
#endif /* __buttonPad */

    pinMode(SparkIntPIN, INPUT);

#ifdef __buttonPad
    leds.setup(4);
    leds.setColor(0, 255, 255, 0);
    leds.setColor(1, 0, 255, 0);
    leds.setColor(2, 0, 0, 255);
    leds.setColor(3, 255, 0, 0);
    leds.show();
    lastLedAction = millis();
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

    mcp.pinMode(BTN_6, INPUT);
    mcp.pullUp(BTN_6, HIGH);

    mcp.pinMode(BTN_7, INPUT);
    mcp.pullUp(BTN_7, HIGH);

#ifdef __gira
    mcp.pinMode(BTN_LED_0, OUTPUT);
    mcp.pinMode(BTN_LED_1, OUTPUT);
    mcp.pinMode(BTN_LED_2, OUTPUT);
    mcp.pinMode(BTN_LED_3, OUTPUT);
    mcp.pinMode(BTN_LED_4, OUTPUT);
    mcp.pinMode(BTN_LED_5, OUTPUT);

    mcp.digitalWrite(BTN_LED_0, LOW);
    mcp.digitalWrite(BTN_LED_1, LOW);
    mcp.digitalWrite(BTN_LED_2, LOW);
    mcp.digitalWrite(BTN_LED_3, LOW);
    mcp.digitalWrite(BTN_LED_4, LOW);
    mcp.digitalWrite(BTN_LED_5, LOW);
#endif /* __gira */   

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

#ifdef SERIAL_DEBUG
    Serial.println("Hello :)");
#endif /* SERIAL_DEBUG */
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

#ifdef __buttonPad
    /* stop LEDs after LED_TIMEOUT seconds */
    if (lastLedAction > 0 && millis() - lastLedAction > LED_TIMEOUT) {
        setLEDs(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
#endif /* __buttonPad */

#ifdef WLAN_DISABLE_RGB
    if (!WiFi.ready()) {
        RGB.control(true);
        RGB.brightness(0);
    }
    if (WiFi.ready() && RGB.controlled()) {
        RGB.brightness(255);
        RGB.control(false);
    }
#endif /* WLAN_DISABLE_RGB */


    if (!btn_event_queue.isEmpty()) {
        t_btn_event _btn_event = btn_event_queue.pop();
        // interpret the event and fire desired action

#ifdef __buttonPad
    #ifdef BUTTONPAD_LED_FEEDBACK        
            /* Set LED Color after Button Press */
            switch (_btn_event.btn) {
                case BTN_4:
                    leds.setColor(2, 0, 0, 255);
                    break;
                case BTN_5:
                    leds.setColor(3, 255, 0, 0);
                    break;
                case BTN_6:
                    leds.setColor(1, 0, 255, 0);
                    break;
                case BTN_7:
                    leds.setColor(0, 255, 255, 0);
                    break;
            }
            leds.show();
            lastLedAction = (millis() - LED_TIMEOUT) + LED_BTN_TIMEOUT;
    #endif /* BUTTONPAD_LED_FEEDBACK */        
#endif /* __buttonPad */
        myConfig.process(&_btn_event);
    }

}


