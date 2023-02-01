# AsyncWebServer_plus_MQTT Mbed OS Crash
Example of Portenta MQTT plus Async Webserver crashing in 1-2 minutes after html web-all

Async Libraries examples using:
https://github.com/khoih-prog/AsyncMQTT_Generic
https://github.com/khoih-prog/Portenta_H7_AsyncWebServer

The examples straight from the Libararies examples, combined in one Sketch
Expecting these could co-exsist, but they dont : Mbed Crash after 1-2 minutes doing frequantweb-calls

Test1: CRASHES
USed libraries :
AsyncMqtt_Generic 1.8.1
Portenta_H7_AsyncWebServer 1.5.0
Portenta_H7_AsyncTCP 1.4.0
# Used Mbed OS : 3.5.4

Test2: CRASHES
USed libraries :
AsyncMqtt_Generic 1.8.1
Portenta_H7_AsyncWebServer 1.5.0
Portenta_H7_AsyncTCP 1.4.0
# Used Mbed OS : 3.5.1

Test3: CRASHES
USed libraries :
AsyncMqtt_Generic 1.8.1
Portenta_H7_AsyncWebServer 1.5.0
Portenta_H7_AsyncTCP 1.4.0
# Used Mbed OS : 3.3.0

Test4: CRASHES
USed libraries :
AsyncMqtt_Generic 1.8.1
Portenta_H7_AsyncWebServer 1.5.0
Portenta_H7_AsyncTCP 1.4.0
# Used Mbed OS : 2.8.0

Test5: CRASHES
USed libraries :
AsyncMqtt_Generic 1.8.1
Portenta_H7_AsyncWebServer 1.5.0
Portenta_H7_AsyncTCP 1.4.0
# Used Mbed OS : 2.5.2


