#include "SONOSClient.h"
#include "spark_http_client/http_client.h"

// for debugging
#define SONOS_SERIAL_DEBUG 1

SONOSClient::SONOSClient()
{
}

void SONOSClient::setIP(unsigned short a, unsigned short b, unsigned short c, unsigned short d) {
  sonosip[0] = a;
  sonosip[1] = b;
  sonosip[2] = c;
  sonosip[3] = d;
}

short SONOSClient::getMute() {
    return getShortValue("Mute", "<CurrentMute>");
}

void SONOSClient::setMute(bool muteit) {
  if (muteit) {
    setShortValue("Mute", 1);   
  } else {
    setShortValue("Mute", 0); 
  }
  
}


void SONOSClient::toggleMute() {
  currentMute = getMute();
  #ifdef SONOS_SERIAL_DEBUG
  Serial.println(currentMute);
  #endif /* SONOS_SERIAL_DEBUG */

  if ( currentMute == 1) {
    #ifdef SONOS_SERIAL_DEBUG
    Serial.println("unmute");
    #endif /* SONOS_SERIAL_DEBUG */

    setMute(FALSE);
  } else {
    #ifdef SONOS_SERIAL_DEBUG
    Serial.println("mute");
    #endif /* SONOS_SERIAL_DEBUG */

    setMute(TRUE);
  }

  
  
}


void SONOSClient::changeVolume(short delta) {
  if (delta != 0 && delta <= 100 && delta >= -100) {
    
    // negative value indicates error or unset state
    if (currentVolume == -1 || (millis()-lastVolumeRead)>__MAX_VOLUME_AGE) {
      #ifdef SONOS_SERIAL_DEBUG
      Serial.println("Updating current Volume");
      #endif /* SONOS_SERIAL_DEBUG */
      currentVolume = getVolume();
      
      // update only after updated
      lastVolumeRead = millis();
    } else {
      #ifdef SONOS_SERIAL_DEBUG
      Serial.println("Using the stored Volume");
      #endif /* SONOS_SERIAL_DEBUG */
    }
    
    #ifdef SONOS_SERIAL_DEBUG
    Serial.print("Current Volume: ");
    Serial.println(currentVolume);
    #endif /* SONOS_SERIAL_DEBUG */
    
    if (currentVolume >= 0) {
      currentVolume = currentVolume + delta;
    
      // upper limit
      if (currentVolume > 100) {
        currentVolume = 100;
      }

      // lower limit
      if (currentVolume < 0) {
        currentVolume = 0;
      }
    
      setVolume(currentVolume);
    } else {
      //Do some error handling
    }
    
  }
}

short SONOSClient::getVolume() {
    return getShortValue("Volume", "<CurrentVolume>");
}

void SONOSClient::setVolume(short newVolume) {
  setShortValue("Volume", newVolume); 
 
}

/*----------------------------------------------------------------------*/
/* Private Methods */


/*----------------------------------------------------------------------*/
/* simple parsing of the responses                                      */
/* find <element>X</element> and return X as a String                   */
String SONOSClient::getXMLElementContent(String input, String element)
{
    // make open and close string
    String elementOpen = "<" + element + ">";
    String elementClose = "</" + element + ">";
    
    // get positions of open and close
    int openPos = input.indexOf(elementOpen);
    int closePos = input.indexOf(elementClose);
    
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


short SONOSClient::getShortFromElement(char *input, short inputlength, const char *tag, short taglength) {
    
    // try to find the tag in the string
    // returns either NULL or pointer to
    // the beginning of our tag
    char *pos = strstr(input, tag);
    
    // if found, and does not end at the end of the string
    if(pos != NULL && (pos - input)<(inputlength + taglength)) {
        // we move the pointer by the length of the tag
        // now we can directly use atoi, as it processes all following
        // numbers, sure to stop when the next tag opens
        return(atoi(pos+taglength));
    }
    
    return NULL;
}

/*----------------------------------------------------------------------*/
// soap set and get

// call like 
// setShortValue("Mute", 0)
void SONOSClient::setShortValue(const char *service, short valueToSet) 
{
  // create the soap action header field
  memset(soapActionHeader, 0, __SOAP_ACTION_HEADER_LENGTH);
  snprintf(soapActionHeader, __SOAP_ACTION_HEADER_LENGTH, "SOAPACTION: \"%s#Set%s",__SOAP_METHOD, service); 
    
  // create the soap body
  memset(soapBody, 0, __SOAP_BODY_LENGTH);
  snprintf(soapBody, __SOAP_BODY_LENGTH, "%s<u:Set%s xmlns:u=\"%s\">%s<Desired%s>%d</Desired%s></u:Set%s>%s", __SOAP_OPEN, service, __SOAP_METHOD, __SOAP_CHANNEL, service, valueToSet, service, service, __SOAP_CLOSE); /*content*/
  
  myHttpClient.makeRequest(1, /* type, 1 = POST */
                    "/MediaRenderer/RenderingControl/Control", /* URL */
                    sonosip, /* host */
                    __SONOS_PORT,  /* port */
                    FALSE, /* KEEP ALIVE */ 
                    "text/xml; charset=\"utf-8\"", /* contentType */
                    soapActionHeader, 
                    "", /* userHeader2 */ 
                    soapBody, /* content */
                    buffer, /* response */
                    REQUEST_LEN, /* response size */
                    FALSE /* storeResponseHeader */
                      );
}

// returns integer value or -1 in case of an error
short SONOSClient::getShortValue(const char *service, const char *responsefield)
{
  // create the soap action header field
  memset(soapActionHeader, 0, __SOAP_ACTION_HEADER_LENGTH);
  snprintf(soapActionHeader, __SOAP_ACTION_HEADER_LENGTH, "SOAPACTION: \"%s#Get%s",__SOAP_METHOD, service); 
  
  // create the soap body
  memset(soapBody, 0, __SOAP_BODY_LENGTH);
  snprintf(soapBody, __SOAP_BODY_LENGTH, "%s<u:Get%s xmlns:u=\"%s\">%s</u:Get%s>\r\n", __SOAP_OPEN, service, __SOAP_METHOD, __SOAP_CHANNEL, service);
  
  
  short received_bytes = myHttpClient.makeRequest(1, /* type, 1 = POST */
                    "/MediaRenderer/RenderingControl/Control", /* URL */
                    sonosip, /* host */
                    __SONOS_PORT,  /* port */
                    FALSE, /* KEEP ALIVE */ 
                    "text/xml; charset=\"utf-8\"", /* contentType */
                    soapActionHeader, 
                    "", /* userHeader2 */ 
                    soapBody, /*content*/
                    buffer, /* response */
                    REQUEST_LEN, /* response size */
                    FALSE /* storeResponseHeader */
                      );

  if (received_bytes > 0) {
    //String returnedValue = getXMLElementContent(buffer, responsefield);
    //if (returnedValue != NULL) {
    //  return returnedValue.toInt();
    //}
//    return parsePositiveShortValueFromBehind(buffer, received_bytes, 60, '<');
    return getShortFromElement(buffer, received_bytes, responsefield, strlen(responsefield));
    
  }

  // otherwise return -1 to indicate an error
  return -1;
  
}
