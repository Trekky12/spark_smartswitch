#include "sonos.h"

/* IP addresses of Arduino and ZonePlayer */
#define IP1     10
#define IP2     0
#define IP3     6
#define IP4ZP   114 /* Office */
#define IP4ARD  118 /* Arduino */

/* Enable DEBUG_SONOS for serial debug output */
//
#define DEBUG_SONOS

/*----------------------------------------------------------------------*/
/* Global variables */
/*----------------------------------------------------------------------*/


/* Ethernet control */
TCPClient       client; //(sonosip, 1400);


/* IP address of ZonePlayer to control */
// byte sonosip[] = {
// 	IP1, IP2, IP3, IP4ZP
// };
// 
byte sonosip[] = { 10, 0, 6, 1 };
byte sonos_ip[] = { 10, 0, 6, 1 };

#define SENDBUFFLEN     350



/* Millisecond timer values */
unsigned long   lastcmd = 0;
unsigned long   lastrew = 0;
unsigned long   lastpoll = 0;

/* Global used to store number of seconds to seek to in a Sonos command */
int             desttime;

/* Global used for volume setting */
int             newvol = 0;

/* Buffers used for Sonos data reception */
char            data1[20];
char            data2[20];

/* Global null buffer used to disable data reception */
char            nullbuf[1] = {0};

//vars copied out of yonos_cmd

char            buf[SENDBUFFLEN];
  
char            cmdbuf[32];
char            extra[64];
char            service[20];
char            *ptr1;
char            *ptr2;
char            *optr;
char            copying;
unsigned long   timeout;





SONOSClient::SONOSClient()
{
}

/*----------------------------------------------------------------------*/
/* out - outputs supplied string to Ethernet client */

void 
out(const char *s)
{
	//client.println(s);
  
  //client.write((const uint8_t*)s,strlen(s));
  client.println(s);
  //client.write((const uint8_t*)s.c_str(),strlen(s));  
  //client.write((const uint8_t*)"asdf", 5);
  //client.print(s);
  
#ifdef DEBUG_SONOS
  Serial.println(s);
    //Serial.write((const uint8_t*)s,strlen(s));  
#endif

  //memset(buf, 0, SENDBUFFLEN);
}




/*----------------------------------------------------------------------*/
/* sonos - sends a command packet to the ZonePlayer without response*/

void
SONOSClient::sonos_cmd(int cmd)
{
  sonos_cmd(cmd, nullbuf, nullbuf);
}


/*----------------------------------------------------------------------*/
/* sonos - sends a command packet to the ZonePlayer */

void 
SONOSClient::sonos_cmd(int cmd, char *resp1, char *resp2)
{
  #ifdef DEBUG_SONOS
    Serial.println("[SONOSClient] INF: In sonos_cmd method");
  #endif
    
    extra[0] = 0;
    memset(buf, 0, SENDBUFFLEN);
    memset(cmdbuf,0, 32);
    memset(extra,0, 64);
    memset(service,0, 20);
    memset(data1, 0, 20);
    memset(data2, 0, 20);
    

	strcpy(service, "AVTransport");
  //here used the if(client... to be...)
	if (client.connect(sonos_ip, 1400)) {
  #ifdef DEBUG_SONOS
    Serial.println("[SONOSClient] INF: Connected");
  #endif
    
		/* wait for a response packet */
		timeout = millis();
    
    /*
		 * prepare the data strings to go into the desired command
		 * packet
		 */
		switch (cmd) {
		case SONOS_PLAY:
			strcpy(cmdbuf, "Play");
			strcpy(extra, "<Speed>1</Speed>");
			break;

		case SONOS_PAUSE:
			strcpy(cmdbuf, "Pause");
			break;

		case SONOS_PREV:
			strcpy(cmdbuf, "Previous");
			break;

		case SONOS_NEXT:
			strcpy(cmdbuf, "Next");
			break;

		case SONOS_SEEK:
			strcpy(cmdbuf, "Seek");
			sprintf(extra, "<Unit>REL_TIME</Unit><Target>%02d:%02d:%02d</Target>", desttime / 3600, (desttime / 60) % 60, desttime % 60);
			break;

		case SONOS_NORMAL:
		case SONOS_REPEAT:
		case SONOS_SHUFF:
		case SONOS_SHUREP:
			if (cmd == SONOS_NORMAL)
				strcpy(cmdbuf, "NORMAL");
			if (cmd == SONOS_REPEAT)
				strcpy(cmdbuf, "REPEAT_ALL");
			if (cmd == SONOS_SHUFF)
				strcpy(cmdbuf, "SHUFFLE_NOREPEAT");
			if (cmd == SONOS_SHUREP)
				strcpy(cmdbuf, "SHUFFLE");
			sprintf(extra, "<NewPlayMode>%s</NewPlayMode>", cmdbuf);
			strcpy(cmdbuf, "SetPlayMode");
			break;

		case SONOS_MODE:
			strcpy(cmdbuf, "GetTransportSettings");
			strcpy(resp1, "PlayMode");
			break;

		case SONOS_POSIT:
			strcpy(cmdbuf, "GetPositionInfo");
			strcpy(resp1, "RelTime");
			break;

		case SONOS_GETVOL:
			strcpy(cmdbuf, "GetVolume");
			strcpy(extra, "<Channel>Master</Channel>");
			strcpy(service, "RenderingControl");
			strcpy(resp1, "CurrentVolume");
			break;

		case SONOS_SETVOL:
			strcpy(cmdbuf, "SetVolume");
			sprintf(extra, "<Channel>Master</Channel><DesiredVolume>%d</DesiredVolume>", newvol);
			strcpy(service, "RenderingControl");
			break;  
    #ifdef DEBUG_SONOS
      default:
      Serial.println("[SONOSClient] ERR: Command not defined");
    #endif
    
		} // end siwtch(cmd)

    // add a short delay before start sending
    //while ((millis() - timeout) < 1000);
    
		/* output the command packet */
    sprintf(buf, "POST /MediaRenderer/%s/Control HTTP/1.1", service);
    out(buf);
    out("Connection: close");
    sprintf(buf, "Host: %d.%d.%d.%d:1400", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
    out(buf);
    sprintf(buf, "Content-Length: %d", 231 + 2 * strlen(cmdbuf) + strlen(extra) + strlen(service));
    out(buf);
    out("Content-Type: text/xml; charset=\"utf-8\"");
    sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:%s:1#%s\"", service, cmdbuf);
    out(buf);
    out("");
    sprintf(buf, "%s<u:%s%s%s%s%s</u:%s>%s", SONOS_CMDH, cmdbuf, SONOS_CMDP, service, SONOS_CMDQ, extra, cmdbuf, SONOS_CMDF);
    
    // <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:SetVolume xmlns:u="urn:schemas-upnp-org:service:RenderingControl:1"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>0</DesiredVolume></u:SetVolume></s:Body></s:Envelope>
    
    out(buf);
    client.flush();
    

    #ifdef DEBUG_SONOS
	    Serial.println("[SONOSClient] INF: All written");
    #endif

    //timeout was 1000, maybe 2000 is better
    
    // SPARK_WLAN_LOOP() needed otherwise there are interrupts done by SPARK CLOUD which can cause problems...
//		while ((!client.available()) && ((millis() - timeout) < 500)); // SPARK_WLAN_Loop();


    #ifdef DEBUG_SONOS
    	Serial.println("[SONOSClient] INF: Client has data available or timeout exceeded");
    #endif
		/*
		 * parse the response looking for the strings in resp1 and
		 * resp2
		 */
		ptr1 = resp1;
		ptr2 = resp2;
		copying = 0;
		//while (client.available()) {

		timeout = millis();
  	while (client.available()) 
    {

      if ((millis()-timeout) < 500) {
      	timeout = millis();
      } else {
        #ifdef DEBUG_SONOS
          Serial.println("[SONOSClient] ERR: Client failed to receive data after timeout");
        #endif
        break;
      }
      
			char c = client.read();
      /*
			 * if response buffers start with nulls, either no
			 * response required, or already received
			 */
			if (resp1[0] || resp2[0]) {
				/*
				 * if a response has been identified, copy
				 * the data
				 */
				if (copying) {
					/*
					 * look for the < character that
					 * indicates the end of the data
					 */
					if (c == '<') {
						/*
						 * stop receiving data, and
						 * null the first character
						 * in the response buffer
						 */
						copying = 0;
						*optr = 0;
						if (copying == 1)
							resp1[0] = 0;
						else
							resp2[0] = 0;
					} else {
						/*
						 * copy the next byte to the
						 * response buffer
						 */
						*optr = c;
						optr++;
					}
				} else {
					/*
					 * look for input characters that
					 * match the response buffers
					 */
					if (c == *ptr1) {
						/*
						 * character matched -
						 * advance to next character
						 * to match
						 */
						ptr1++;

						/*
						 * is this the end of the
						 * response buffer
						 */
						if (*ptr1 == 0) {
							/*
							 * string matched -
							 * start copying from
							 * next character
							 * received
							 */
							copying = 1;
							optr = resp1;
							ptr1 = resp1;
						}
					} else
						ptr1 = resp1;

					/*
					 * as above for second response
					 * buffer
					 */
					if (c == *ptr2) {
						ptr2++;

						if (*ptr2 == 0) {
							copying = 2;
							optr = resp2;
							ptr2 = resp2;
						}
					} else
						ptr2 = resp2;
				}
			}
#ifdef DEBUG_SONOS
//			Serial.print(c);
#endif
		
    } // end while
    #ifdef DEBUG_SONOS
  		Serial.println("[SONOSClient] INF: client no longer available");
    #endif
	} else {
#ifdef DEBUG_SONOS
		Serial.println("[SONOSClient] INF: connection failed");
#endif
	} // end if connect
  
  // tcp client seems to have a problem getting closed to early
  timeout = millis();
  while ((millis() - timeout) < 50); //  SPARK_WLAN_Loop();
  client.flush();

  timeout = millis();
  while ((millis() - timeout) < 50); //  SPARK_WLAN_Loop();
	client.stop();

#ifdef DEBUG_SONOS
		Serial.println("[SONOSClient] INF: Connection closed");
#endif
}


void SONOSClient::sonos_setVol(int volToSet)
{
  #ifdef DEBUG_SONOS
	  Serial.println("[SONOSClient] INF: In setVol method");
  #endif
  newvol = volToSet;
  sonos_cmd(SONOS_SETVOL);
}