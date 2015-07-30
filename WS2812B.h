#ifndef __WS2812B_H
#define __WS2812B_H

#include "application.h"

typedef struct {
    unsigned int red:8;
    unsigned int green:8;
    unsigned int blue:8;
  } __attribute((packed)) RGBPixel;

class WS2812B 
{

public:
	WS2812B();
	virtual void setup(uint16_t _numLeds);
	virtual void setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue);
	virtual void show();

};


#endif /* __WS2812B_H */