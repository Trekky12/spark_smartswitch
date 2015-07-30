#ifndef __HTTPGET_H
#define __HTTPGET_H

#include "application.h"
#include "http_client.h"


/*----------------------------------------------------------------------*/
/* Macros and constants */
/*----------------------------------------------------------------------*/


// Buffer definitions

#define RESPONSE_LEN 1024
#define REQUEST_LEN  800


// TODO abstract class / interface definieren

class HTTPGETClient 
{
  
HTTPClient myHttpClient;

// response buffer
char buffer[RESPONSE_LEN];
static const uint16_t TIMEOUT = 5000; // Allow maximum 5s between data packets.

//#define SONOS_LOGING
byte hostip[4] = { 0 };
unsigned short hostport = 0;

  

public:
	HTTPGETClient();
  
  // setIP 
  virtual void   setIP(unsigned short a, unsigned short b, unsigned short c, unsigned short d);
  
  // setPort
  virtual void   setPort(unsigned short port);

  // make request
  virtual void sendRequest(const char *url, const char *message);
  

};




#endif /* __HTTPGET_H */