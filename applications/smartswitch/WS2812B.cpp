#include "WS2812B.h"


uint16_t numLeds = 4; // number of LEDs
RGBPixel *pixelBufferP; // the pixel buffer  

WS2812B::WS2812B()
{

}

void WS2812B::setup(uint16_t _numLeds){
    numLeds = _numLeds;
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8); // System clock is 72MHz, we need 9MHz for SPI
    SPI.setBitOrder(MSBFIRST); // MSB first for easier scope reading :-)
    SPI.transfer(0); // make sure SPI line starts low (Note: SPI line remains at level of last sent bit, fortunately)
  // begin using the driver

  if((pixelBufferP = new RGBPixel[numLeds])!=NULL) {
    memset(pixelBufferP, 0, sizeof(RGBPixel)*numLeds); // all LEDs off
  }
}



void
WS2812B::setColor(uint16_t aLedNumber, byte aRed, byte aGreen, byte aBlue)
{
  if (aLedNumber>=numLeds) return; // invalid LED number
  RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
  // linear brightness is stored with 5bit precision only
  pixP->red = aRed;
  pixP->green = aGreen;
  pixP->blue = aBlue;
}

uint8_t getR(uint16_t aLedNumber){
    if (aLedNumber>=numLeds) return 0; // invalid LED number
    RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
    
    Serial.println(pixP->red);
    return (pixP->red);
}

uint8_t getG(uint16_t aLedNumber){
    if (aLedNumber>=numLeds) return 0; // invalid LED number
    RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
    
    return (pixP->green);
}
uint8_t getB(uint16_t aLedNumber){
    if (aLedNumber>=numLeds) return 0; // invalid LED number
    RGBPixel *pixP = &(pixelBufferP[aLedNumber]);
    
    return (pixP->blue);
}


void WS2812B::show()
{
  // Note: on the spark core, system IRQs might happen which exceed 50uS
  // causing WS2812 chips to reset in midst of data stream.
  // Thus, until we can send via DMA, we need to disable IRQs while sending
  __disable_irq();
  // transfer RGB values to LED chain
  for (uint16_t i=0; i<numLeds; i++) {
    RGBPixel *pixP = &(pixelBufferP[i]);
    byte b;
    // Order of PWM data for WS2812 LEDs is G-R-B
    // - green
    b = pixP->green;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - red
    b = pixP->red;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
    // - blue
    b = pixP->blue;
    for (byte j=0; j<8; j++) {
      SPI.transfer(b & 0x80 ? 0x7E : 0x70);
      b = b << 1;
    }
  }
  __enable_irq();
}