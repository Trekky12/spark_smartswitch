#ifndef __SONOS_H
#define __SONOS_H

#include "allduino.h"
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

#define REQUEST_LEN 800
#define __SONOS_PORT 1400


class SONOSClient 
{

public:
	SONOSClient();

  // simple mute control
  virtual short getMute();
  virtual void setMute(bool muteit);
  
  // more complex control
  // first get the current state, then invert
  virtual void toggleMute();
  

  // simple volume control
  virtual short  getVolume();
  virtual void   setVolume(short newVolume);
  
  virtual void   changeVolume(short delta);
  

    
private:
  String getXMLElementContent(String input, String element);
  void  setShortValue( const char *variableToSet, short valueToSet) ;
  short getShortValue( const char *variableToGet, const char *responsefield);

};




#endif /* __SONOS_H */