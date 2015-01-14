#ifndef __CONFIG_H
#define __CONFIG_H

#include "application.h"
// include what is needed

// Define eventtypes and buttons
// btn can be one specific button or a sequence of buttons

enum btn {
    BTN_0 = 0, BTN_1 = 1, BTN_2 = 2, BTN_3 = 3, BTN_4 = 4, BTN_5 = 5, BTN_6 = 6, BTN_SEQUENCE = 7
};

// btn_event_type can be based one one button or a specific sequence
enum btn_event_type {
    BTN_SINGLE, BTN_DOUBLE, BTN_TRIPLE, BTN_HOLD, SEQUENCE_1, SEQUENCE_2
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
  virtual void process(t_btn_event* e);

};

#endif /* __CONFIG_H */