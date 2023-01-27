/****************************************************************************************************************************
  defines.h

  For Portenta_H7 (STM32H7) with Vision-Shield Ethernet

  Portenta_H7_AsyncWebServer is a library for the Portenta_H7 with with Vision-Shield Ethernet

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/Portenta_H7_AsyncWebServer
  Licensed under GPLv3 license
 *****************************************************************************************************************************/


#ifndef defines_h
#define defines_h

#if !( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) )
	#error For Portenta_H7 only
#endif

#define _PORTENTA_H7_AWS_LOGLEVEL_     1
#define USE_WIFI_PORTENTA_H7        true


char ssid[] = "SID";        // your network SSID (name)
char pass[] = "pass";     // your network password (use for WPA, or use as key for WEP), length must be 8+


#endif    //defines_h
