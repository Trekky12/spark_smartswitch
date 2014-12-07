// This #include statement was automatically added by the Spark IDE.
#include "MCP23017.h"
#include "http_client.h"

SYSTEM_MODE(SEMI_AUTOMATIC);

Adafruit_MCP23017 mcp;



#define __SONOS_PORT 1400
#define REQUEST_LEN 800


//
char buffer[1024];
char request[REQUEST_LEN];
char header[400];
char linebuffer[85];


unsigned int bufferPosition = 0;
unsigned long lastRead = millis();
unsigned long firstRead = millis();
bool error = false;
bool timeout = false;
//

enum btn  {BTN_1=1, BTN_2=2, BTN_3=3, BTN_4=4, BTN_5=5, BTN_6=6, BTN_SEQUENCE=7};
int LED = D7;
byte SparkIntPIN=D3;



unsigned long last_sent=0L;
TCPClient       client; 

HTTPClient myHttpClient;

byte serverIP[] = { 10, 0, 6, 118 };

int r=0;

volatile int doIT = 0;

void handleButtonINT() {
    
    doIT = 1;
    mcp.readGPIOAB();
    
}



void setup() {
    

  delay(1000);
  
  WiFi.connect();

  while (!WiFi.ready()) SPARK_WLAN_Loop();
  
  
  
  
  pinMode(SparkIntPIN, INPUT);
  // Spark Interupt 
  attachInterrupt(SparkIntPIN,handleButtonINT,FALLING);
  pinMode(LED, OUTPUT);
  
  
  
  mcp.begin();      // use default address 0
  
  mcp.pinMode(BTN_1, INPUT);
  mcp.pullUp(BTN_1, HIGH); 
  
  mcp.pinMode(BTN_2, INPUT);
  mcp.pullUp(BTN_2, HIGH); 
  
  
  mcp.setupInterrupts(true,false,LOW);

  mcp.setupInterruptPin(BTN_1,CHANGE);
  mcp.setupInterruptPin(BTN_2,CHANGE);

  mcp.readGPIOAB();
  
  

}



void mute(bool muteit) {
  
  if(muteit) {
    myHttpClient.makeRequest(1, /* type, 1 = POST */
                      "/MediaRenderer/RenderingControl/Control", /* URL */
                      serverIP, /* host */
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
                      serverIP, /* host */
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




void makeHeader( char* header, byte* host, int contentLength, const char* soapaction) 
{
  
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  memset(header, 0, sizeof(header));
    
  sprintf(header, "POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
  strcat(header,  "CONNECTION: close\r\n");
  strcat(header,  "ACCEPT-ENCODING: gzip\r\n");
  
  snprintf(linebuffer, 85, "HOST: %d.%d.%d.%d:%d\r\n", host[0], host[1], host[2], host[3], __SONOS_PORT);
  strcat(header, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
  
  strcat(header,  "USER-AGENT: Linux UPnP/1.0 Sonos/27.2-81200 (MDCR_MacPro4,1)\r\n");
  
  snprintf(linebuffer, 85, "CONTENT-LENGTH: %d\r\n" , contentLength);
  strcat(header, linebuffer);
  memset(&linebuffer, 0, sizeof(linebuffer));
  
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




void loop() {


if(millis()-last_sent>80) {
  
  mute(TRUE);

  last_sent=millis();

}

}

