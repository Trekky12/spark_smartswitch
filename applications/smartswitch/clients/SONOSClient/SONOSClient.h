#ifndef __SONOS_H
#define __SONOS_H

#include "../../lib/allduino.h"
//#include <stdlib.h>
#include "spark_wiring_tcpclient.h"
#include "application.h"
#include "spark_http_client/http_client.h"


/*----------------------------------------------------------------------*/
/* Macros and constants */
/*----------------------------------------------------------------------*/

// static fragments of the soap requests
#define __SOAP_OPEN     "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>"
#define __SOAP_CLOSE    "</s:Body></s:Envelope>"
#define __SOAP_METHOD   "urn:schemas-upnp-org:service:RenderingControl:1"
#define __SOAP_CHANNEL  "<InstanceID>0</InstanceID><Channel>Master</Channel>"

#define __SOAP_ACTION_HEADER_LENGTH 85
#define __SOAP_BODY_LENGTH 350

#define __SONOS_PORT 1400


// Buffer definitions

#define RESPONSE_LEN 1024
#define REQUEST_LEN 800


// TODO abstract class / interface definieren

class SONOSClient 
{

  /*----------------------------------------------------------------------*/
  /* Global variables */
  /*----------------------------------------------------------------------*/



  HTTPClient myHttpClient;

  // response buffer
  char buffer[RESPONSE_LEN];
  char request[REQUEST_LEN];

  char linebuffer[85];

  char soapActionHeader[__SOAP_ACTION_HEADER_LENGTH];
  char soapBody[__SOAP_BODY_LENGTH];

  // maximum time the stored volume is accepted
  // after time expired, volume will be fetched from the remote
  // intention is to reduce request number especially when 
  // user tries to increase/decrease the volume multiple times
  // the propability that this method interferes with volume
  // changes of other sources can be seen as quite small
  #define __MAX_VOLUME_AGE 1500
  short currentVolume = -1;
  unsigned long lastVolumeRead = 0L;

  short currentMute = 0;


  static const uint16_t TIMEOUT = 5000; // Allow maximum 5s between data packets.

  //#define SONOS_LOGING

  byte sonosip[4] = { 0 };
  
  

public:
	SONOSClient();
  
  // setIP 
  virtual void   setIP(unsigned short a, unsigned short b, unsigned short c, unsigned short d);

  // simple mute control
  virtual short  getMute();
  virtual void   setMute(bool muteit);
  
  // more complex control
  // first get the current state, then invert
  virtual void   toggleMute();
  
  // simple volume control
  virtual short  getVolume();
  virtual void   setVolume(short newVolume);
  
  virtual void   changeVolume(short delta);
  
private:
  String getXMLElementContent(String input, String element);
  void  setShortValue( const char *variableToSet, short valueToSet) ;
  short getShortValue( const char *variableToGet, const char *responsefield);
  short getShortFromElement(char *input, short inputlength, const char *tag, short taglength);

};




#endif /* __SONOS_H */