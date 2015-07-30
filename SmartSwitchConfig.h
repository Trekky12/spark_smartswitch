#ifndef __SMARTSWITCHCONFIG_H
#define __SMARTSWITCHCONFIG_H


#include "application.h"
// include what is needed
#include "neopixel.h"

// Define eventtypes and buttons
// btn can be one specific button or a sequence of buttons

enum btn {
    BTN_0 = 0, BTN_1 = 1, BTN_2 = 2, BTN_3 = 3, BTN_4 = 4, BTN_5 = 5, BTN_6 = 6, BTN_7 = 7
};

enum btn_leds {
    BTN_LED_0 = 10, BTN_LED_1 = 11, BTN_LED_2 = 12, BTN_LED_3 = 13, BTN_LED_4 = 14, BTN_LED_5 = 15
};

// btn_event_type can be based one one button or a specific sequence
enum btn_event_type {
    BTN_SINGLE, BTN_DOUBLE, BTN_HOLD_CLICK, BTN_HOLD
};


typedef struct {
    uint8_t btn;
    uint8_t event;
} t_btn_event;


class SMARTSWITCHConfig
{

public:
	SMARTSWITCHConfig();

  virtual void setup();
  virtual void process(t_btn_event* e, Adafruit_NeoPixel* strip);

};

#endif /* __SMARTSWITCHCONFIG_H */
