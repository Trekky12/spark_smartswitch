#ifndef __SONOS_H
#define __SONOS_H

#include "allduino.h"
//#include <stdlib.h>
#include "spark_wiring_tcpclient.h"
#include "application.h"


/*----------------------------------------------------------------------*/
/* Macros and constants */
/*----------------------------------------------------------------------*/

/* Sonos SOAP command packet skeleton */
#define SONOS_CMDH "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>"
#define SONOS_CMDP " xmlns:u=\"urn:schemas-upnp-org:service:"
#define SONOS_CMDQ ":1\"><InstanceID>0</InstanceID>"
#define SONOS_CMDF "</s:Body></s:Envelope>"

/* Sonos SOAP command packet enumeration */
#define SONOS_PAUSE  0
#define SONOS_PLAY   1
#define SONOS_PREV   2
#define SONOS_NEXT   3
#define SONOS_SEEK   4
#define SONOS_NORMAL 5
#define SONOS_REPEAT 6
#define SONOS_SHUFF  7
#define SONOS_SHUREP 8
#define SONOS_MODE   9
#define SONOS_POSIT  10
#define SONOS_GETVOL 11
#define SONOS_SETVOL 12


#define __SONOS_PORT 1400






class SONOSClient 
{

public:
	SONOSClient();
  //virtual int connect(IPAddress ip, uint16_t port);
  virtual void sonos_cmd(int cmd);
  virtual void sonos_cmd(int cmd, char *resp1, char *resp2);
  virtual void sonos_setVol(int volToSet);
  virtual void mute(bool muteit);
  virtual void getVol();
  virtual void getMute();
  virtual void toggleMute();
  

};


#endif /* __SONOS_H */