#include "sonos.h"
#include "spark_http_client/http_client.h"

/* Enable DEBUG_SONOS for serial debug output */
//
#define DEBUG_SONOS

#define REQUEST_LEN 800


/*----------------------------------------------------------------------*/
/* Global variables */
/*----------------------------------------------------------------------*/


/* Ethernet control */
unsigned long last_sent=0L;
TCPClient       client; 

HTTPClient myHttpClient;


// response buffer
char buffer[1024];
char request[REQUEST_LEN];
char header[400];
char linebuffer[85];


unsigned int bufferPosition = 0;
unsigned long lastRead = millis();
unsigned long firstRead = millis();
bool error = false;
bool timeout = false;


static const uint16_t TIMEOUT = 5000; // Allow maximum 5s between data packets.

//#define SONOS_LOGING

byte sonosip[] = { 10, 0, 6, 118 };

int r=0;




SONOSClient::SONOSClient()
{
}


// # simple parsing of the responses ------------------------------
//
// find <element>X</element> and return X as a String
//

String getElementContent(String input, String element)
{
    // make open and close string
    String elementOpen = "<" + element + ">";
    String elementClose = "</" + element + ">";
    
    // get positions of open and close
    int openPos = input.indexOf(elementOpen);
    int closePos = input.indexOf(elementClose);
    
    #ifdef SERIALDEBUGPARSER
        Serial.print("[INF] Input length: ");
        Serial.println(input.length());
        Serial.println("[INF] Elements:");
        Serial.println(elementOpen);
        Serial.println(elementClose);
        Serial.println("[INF] Positions:");
        Serial.println(openPos);
        Serial.println(closePos);
    #endif // SERIALDEBUG
    
	
    // verify if open and close can be found in the input
    if (openPos == -1 || closePos == -1) {
		Serial.println("[ERR] can not find element in input");
		return NULL;
	}
    
    // idx: starts at index of element + length of element + tags
    int idx = openPos + elementOpen.length();
 
    // check if length is above 0
    if (closePos - idx > 0) {
        #ifdef SERIALDEBUGPARSER
            Serial.print("[INF] Output length: ");
            Serial.println((input.substring(idx,  closePos)).length());
        #endif
        return input.substring(idx,  closePos);

    } else {
        #ifdef SERIALDEBUGPARSER
            Serial.println("[ERR] idx seems wrong");
        #endif
        return NULL;
        
    }
    
}


/*----------------------------------------------------------------------*/
/* sonos - sends a command packet to the ZonePlayer without response*/








void makeHeader( char* header, byte* host, int contentLength, const char* soapaction) 
{
  
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  memset(header, 0, sizeof(header));
    
  sprintf(header, "POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
  strcat(header,  "CONNECTION: close\r\n");
  //strcat(header,  "ACCEPT-ENCODING: gzip\r\n");
  
  snprintf(linebuffer, 85, "HOST: %d.%d.%d.%d:%d\r\n", host[0], host[1], host[2], host[3], __SONOS_PORT);
  strcat(header, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  //strcat(header,  "USER-AGENT: Linux UPnP/1.0 Sonos/27.2-81200 (MDCR_MacPro4,1)\r\n");
  
  snprintf(linebuffer, 85, "CONTENT-LENGTH: %d\r\n" , contentLength);
  strcat(header, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  //strcat(header,  "CONTENT-LENGTH\r\n");
  
  
  strcat(header,  "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n");
  
  snprintf(linebuffer, 85, "SOAPACTION: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n",  soapaction);
  strcat(header, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  strcat(header,  "\r\n");

}


void makeMuteRequest( char *request, bool muteit)
{
  
  //memset(request, 0, sizeof(request));
  
  strcat(request,  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">");
  strcat(request,  "<s:Body>");
  strcat(request,  "<u:SetMute xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\">");
  strcat(request,  "<InstanceID>0</InstanceID>");
  strcat(request,  "<Channel>Master</Channel>");
  strcat(request,  "<DesiredMute>");
  
  if (muteit) {
    strcat(request,  "1");
    
  } else {
    strcat(request,  "0");
    
  }
  strcat(request,  "</DesiredMute>");
  strcat(request,  "</u:SetMute>");
  strcat(request,  "</s:Body>");
  strcat(request,  "</s:Envelope>");
}


void makeGetRequest( char *request, const char* variableToGet)
{
  
  memset(request, 0, sizeof(request));
  
  strcat(request,  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">");
  strcat(request,  "<s:Body>");
  //strcat(request,  "<u:SetMute xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\">");
  
  snprintf(linebuffer, 85, "<u:%s xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\">", variableToGet);
  strcat(request, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));

  snprintf(linebuffer, 85, "<InstanceID>0</InstanceID><Channel>Master</Channel></u:%s>", variableToGet);
  strcat(request, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
}




void SONOSClient::mute(bool muteit) {
  
  if(muteit) {
    myHttpClient.makeRequest(1, /* type, 1 = POST */
                      "/MediaRenderer/RenderingControl/Control", /* URL */
                      sonosip, /* host */
                      __SONOS_PORT,  /* port */
                      FALSE, /* KEEP ALIVE */ 
                      "text/xml; charset=\"utf-8\"", /* contentType */
                      "SOAPACTION: \"urn:schemas-upnp-org:service:RenderingControl:1#SetMute", 
                      "", /* userHeader2 */ 
                      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetMute xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredMute>1</DesiredMute></u:SetMute></s:Body></s:Envelope>", /*content*/
                      buffer, /* response */
                      1023 /* response size */
                        );
  }else {
    myHttpClient.makeRequest(1, /* type, 1 = POST */
                      "/MediaRenderer/RenderingControl/Control", /* URL */
                      sonosip, /* host */
                      __SONOS_PORT,  /* port */
                      FALSE, /* KEEP ALIVE */ 
                      "text/xml; charset=\"utf-8\"", /* contentType */
                      "SOAPACTION: \"urn:schemas-upnp-org:service:RenderingControl:1#SetMute", 
                      "", /* userHeader2 */ 
                      "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetMute xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredMute>0</DesiredMute></u:SetMute></s:Body></s:Envelope>", /*content*/
                      buffer, /* response */
                      1023 /* response size */
                        );
  }

}

// dont use methods below

void
SONOSClient::sonos_cmd(int cmd)
{
//  sonos_cmd(cmd, nullbuf, nullbuf);
}


void 
  mute_midold(bool muteit)
{
  
  if(millis()-last_sent>80) {

  if (client.connect(sonosip, 1400)) {  
   
      uint32_t startTime = millis();
     
      makeHeader(request, sonosip, 314, "SetMute");
      makeMuteRequest(request, muteit);

      client.write((const uint8_t *)request, strlen(request));
      client.flush();
    
      while(!client.available() && (millis() - startTime) < 5000){
          SPARK_WLAN_Loop();
      };
    
    
      while(client.available()) {
          r = client.read((uint8_t*) buffer, 1023);
          if (r == -1) break;
      }
    
      client.flush();
      client.stop();
  
  } else {
      // there seems to be something left from a previous connection
      client.flush();
      client.stop();
  }
  
  last_sent=millis();

  }





}


int 
  getValue(char *requestfield, char *responsefield)
{
  makeGetRequest(request, requestfield);
  makeHeader(header, sonosip, strlen(request), requestfield);


  
	if (client.connect(sonosip, 1400)) {  
  
    Serial.println("Connected");
  
    client.write((const uint8_t *)header, strlen(header));
  
    // we dont care about anwsers to the header
    // so we du flush the client prior the first data
    // This seems to be very important
    client.flush();
    
    
    client.write((const uint8_t *)request, strlen(request));
    
    
    //
    // Receive HTTP Response
    //
    // The first value of client.available() might not represent the
    // whole response, so after the first chunk of data is received instead
    // of terminating the connection there is a delay and another attempt
    // to read data.
    // The loop exits when the connection is closed, or if there is a
    // timeout or an error.

    bufferPosition = 0;
    lastRead = millis();
    firstRead = millis();
    error = false;
    timeout = false;

    do {
        #ifdef SONOS_LOGING
        int bytes = client.available();
        if(bytes) {
            Serial.print("\r\nHttpClient>\tReceiving TCP transaction of ");
            Serial.print(bytes);
            Serial.println(" bytes.");
        }
        #endif

        while (client.available()) {
            char c = client.read();
            #ifdef SONOS_LOGING
            Serial.print(c);
            #endif
            lastRead = millis();

            if (c == -1) {
                error = true;

                #ifdef SONOS_LOGING
                Serial.println("HttpClient>\tError: No data available.");
                #endif

                break;
            }

            // Check that received character fits in buffer before storing.
            if (bufferPosition < sizeof(buffer)-1) {
                buffer[bufferPosition] = c;
            } else if ((bufferPosition == sizeof(buffer)-1)) {
                buffer[bufferPosition] = '\0'; // Null-terminate buffer
                client.stop();
                error = true;

                #ifdef SONOS_LOGING
                Serial.println("HttpClient>\tError: Response body larger than buffer.");
                #endif
            }
            bufferPosition++;
        }
        buffer[bufferPosition] = '\0'; // Null-terminate buffer

        #ifdef SONOS_LOGING
        if (bytes) {
            Serial.print("\r\nHttpClient>\tEnd of TCP transaction.");
        }
        #endif

        // Check that there hasn't been more than 5s since last read.
        timeout = millis() - lastRead > TIMEOUT;

        // Unless there has been an error or timeout wait 200ms to allow server
        // to respond or close connection.
        if (!error && !timeout) {
            delay(200);
        }
    } while (client.connected() && !timeout && !error);
    

    
    
      #ifdef LOGGING
        if (timeout) {
            Serial.println("\r\nHttpClient>\tError: Timeout while reading response.");
        }
      #endif
        Serial.print("\r\nHttpClient>\tEnd of HTTP Response (");
        Serial.print(millis() - firstRead);
        Serial.println("ms).");
      client.stop();

      String raw_response(buffer);  
      
      // Not super elegant way of finding the status code, but it works.
      // String statusCode = raw_response.substring(0,50);
      
      String currVol = getElementContent(raw_response, responsefield);
      
      return atoi(currVol.c_str());
    
    
  } else {
    Serial.println("Unable to connnect");
  }
}


void 
  SONOSClient::getVol()
{
  
  Serial.println(getValue("GetVolume", "CurrentVolume"));
 
}

void
  SONOSClient::getMute()
{
  Serial.println(getValue("GetMute", "CurrentMute"));
}

void 
  SONOSClient::toggleMute()
{
  if(getValue("GetMute", "CurrentMute") == 0)
  {
    mute(TRUE);
  } else { 
    mute(FALSE);    
  }
}


void
  getVolOld()
{
  
  
  char request[REQUEST_LEN];
  memset(&request, 0, sizeof(request));
  
	if (client.connect(sonosip, 1400)) {  
  
    Serial.println("Connected");
  
  sprintf(request, "POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
  strcat(request,  "CONNECTION: close\r\n");
  strcat(request,  "ACCEPT-ENCODING: gzip\r\n");
  strcat(request,  "HOST: 10.0.6.118:1400\r\n");
  strcat(request,  "USER-AGENT: Linux UPnP/1.0 Sonos/27.2-81200 (MDCR_MacPro4,1)\r\n");
  strcat(request,  "CONTENT-LENGTH: 270\r\n");
  strcat(request,  "CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n");
  strcat(request,  "SOAPACTION: \"urn:schemas-upnp-org:service:RenderingControl:1#GetVolume\"\r\n");
  strcat(request,  "\r\n");
  
  
  int bufferLength = strlen(request);
  client.write((const uint8_t *)request, bufferLength);
  
  memset(&request, 0, sizeof(request));
  
  // we dont care about anwsers to the header
  // so we du flush the client prior the first data
  // This seems to be very important
  //client.flush();
  
  strcat(request,  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">");
  strcat(request,  "<s:Body>");
  //strcat(request,  "<u:SetMute xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\">");
  strcat(request,    "<u:GetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel></u:GetVolume>");
  strcat(request,  "\r\n");
  
  
    bufferLength = strlen(request);
    Serial.print("Buffer length: ");
    Serial.println(bufferLength);
    
    client.write((const uint8_t *)request, bufferLength);
    

    // RECEIVE RESPONSE
    
    
    // clear response buffer
    memset(&buffer[0], 0, sizeof(buffer));


    //
    // Receive HTTP Response
    //
    // The first value of client.available() might not represent the
    // whole response, so after the first chunk of data is received instead
    // of terminating the connection there is a delay and another attempt
    // to read data.
    // The loop exits when the connection is closed, or if there is a
    // timeout or an error.

    unsigned int bufferPosition = 0;
    unsigned long lastRead = millis();
    unsigned long firstRead = millis();
    bool error = false;
    bool timeout = false;

    do {
        #ifdef SONOS_LOGING
        int bytes = client.available();
        if(bytes) {
            Serial.print("\r\nHttpClient>\tReceiving TCP transaction of ");
            Serial.print(bytes);
            Serial.println(" bytes.");
        }
        #endif

        while (client.available()) {
            char c = client.read();
            #ifdef SONOS_LOGING
            Serial.print(c);
            #endif
            lastRead = millis();

            if (c == -1) {
                error = true;

                #ifdef SONOS_LOGING
                Serial.println("HttpClient>\tError: No data available.");
                #endif

                break;
            }

            // Check that received character fits in buffer before storing.
            if (bufferPosition < sizeof(buffer)-1) {
                buffer[bufferPosition] = c;
            } else if ((bufferPosition == sizeof(buffer)-1)) {
                buffer[bufferPosition] = '\0'; // Null-terminate buffer
                client.stop();
                error = true;

                #ifdef SONOS_LOGING
                Serial.println("HttpClient>\tError: Response body larger than buffer.");
                #endif
            }
            bufferPosition++;
        }
        buffer[bufferPosition] = '\0'; // Null-terminate buffer

        #ifdef SONOS_LOGING
        if (bytes) {
            Serial.print("\r\nHttpClient>\tEnd of TCP transaction.");
        }
        #endif

        // Check that there hasn't been more than 5s since last read.
        timeout = millis() - lastRead > TIMEOUT;

        // Unless there has been an error or timeout wait 200ms to allow server
        // to respond or close connection.
        if (!error && !timeout) {
            delay(200);
        }
    } while (client.connected() && !timeout && !error);
    
    
    
    
      #ifdef LOGGING
        if (timeout) {
            Serial.println("\r\nHttpClient>\tError: Timeout while reading response.");
        }
        Serial.print("\r\nHttpClient>\tEnd of HTTP Response (");
        Serial.print(millis() - firstRead);
        Serial.println("ms).");
      #endif
      client.stop();

      String raw_response(buffer);  
      
      // Not super elegant way of finding the status code, but it works.
      String statusCode = raw_response.substring(0,50);
      
      String currVol = getElementContent(raw_response, "CurrentVolume");
      Serial.print("Current Volume: ");
      Serial.println(currVol);

      #ifdef LOGGING
      Serial.print("HttpClient>\t first 50 characters of response: ");
      Serial.println(statusCode);
      #endif
    
    } else {
    Serial.println("Unable to connnect");
  }
 
}



/*----------------------------------------------------------------------*/
/* sonos - sends a command packet to the ZonePlayer */

void 
SONOSClient::sonos_cmd(int cmd, char *resp1, char *resp2)
{
  #ifdef DEBUG_SONOS
    Serial.println("[SONOSClient] INF: In sonos_cmd method");
  #endif
}


void SONOSClient::sonos_setVol(int volToSet)
{
  #ifdef DEBUG_SONOS
	  Serial.println("[SONOSClient] INF: In setVol method");
  #endif
  sonos_cmd(SONOS_SETVOL);
}