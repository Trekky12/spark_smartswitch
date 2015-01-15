#include "HTTPGETClient.h"


/*----------------------------------------------------------------------*/
/* Global variables */
/*----------------------------------------------------------------------*/





HTTPGETClient::HTTPGETClient()
{
}

void HTTPGETClient::setIP(unsigned short a, unsigned short b, unsigned short c, unsigned short d) {
  hostip[0] = a;
  hostip[1] = b;
  hostip[2] = c;
  hostip[3] = d;
}

void HTTPGETClient::setPort(unsigned short port) {
  hostport = port;
}


void HTTPGETClient::sendRequest(const char *url, const char *message) {
  
    myHttpClient.makeRequest(1, /* type, 1 = POST */
                      url, /* URL */
                      hostip, /* host */
                      hostport,  /* port */
                      FALSE, /* KEEP ALIVE */ 
                      "text/xml; charset=\"utf-8\"", /* contentType */
                      "", 
                      "", /* userHeader2 */ 
                      message, /* content */
                      buffer, /* response */
                      REQUEST_LEN, /* response size */
                      FALSE /* storeResponseHeader */
                        );
}
