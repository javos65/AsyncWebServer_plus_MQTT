/****************************************************************************************************************************
  defines.h

 ***************************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#if !( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) )
  #error For Portenta_H7 only
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define ASYNC_MQTT_DEBUG_PORT               Serial
#define _ASYNC_MQTT_LOGLEVEL_               1

#define USE_ETHERNET_PORTENTA_H7            false
#define USE_WIFI_PORTENTA_H7                true

#include <WiFi.h>

#if (_ASYNC_MQTT_LOGLEVEL_ > 3)
  #warning Using WiFi for Portenta_H7
#endif

#define WIFI_SSID         "ssid"    // your network SSID (name)
#define WIFI_PASSWORD     "pass"    // your network password (use for WPA, or use as key for WEP), length must be 8+

int status = WL_IDLE_STATUS;

#endif    //defines_h
