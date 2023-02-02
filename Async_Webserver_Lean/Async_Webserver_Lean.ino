/****************************************************************************************************************************
  Async_AdvancedWebServer.ino - Dead simple AsyncWebServer for Portenta_H7

  For Portenta_H7 (STM32H7) with Vision-Shield Ethernet or Murata WiFi
  Portenta_H7_AsyncWebServer is a library for the Portenta_H7 with Vision-Shield Ethernet or Murata WiFi
 
 Lean version with one inlin called HomePage
 Test version - never fails 
 *****************************************************************************************************************************/

#if !( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) )
	#error For Portenta_H7 only
#endif

#define _PORTENTA_H7_AWS_LOGLEVEL_     1

#define USE_WIFI_PORTENTA_H7        true

#include <WiFi.h>
#warning Using WiFi for Portenta_H7.

#include <Portenta_H7_AsyncWebServer.h>

char ssid[] = "ssid";        // your network SSID (name)
char pass[] = "pass";         // your network password (use for WPA, or use as key for WEP), length must be 8+

int status = WL_IDLE_STATUS;

AsyncWebServer    server(80);

int reqCount = 0;                // number of requests received

#define LED_OFF             HIGH
#define LED_ON              LOW

void handleNotFound(AsyncWebServerRequest *request)
{
	digitalWrite(LED_BUILTIN, LED_ON);
	String message = "File Not Found\n\n";

	message += "URI: ";
	message += request->url();
	message += "\nMethod: ";
	message += (request->method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += request->args();
	message += "\n";

	for (uint8_t i = 0; i < request->args(); i++)
	{
		message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
	}

	request->send(404, "text/plain", message);
	digitalWrite(LED_BUILTIN, LED_OFF);
}


void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");	Serial.println(WiFi.SSID());
	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("Local IP Address: ");	Serial.println(ip);
	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");	Serial.print(rssi);	Serial.println(" dBm");
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LED_OFF);
	Serial.begin(115200);
	while (!Serial && millis() < 5000);

	delay(200);
	Serial.print("\nStart Async_AdvancedWebServer on ");	Serial.print(BOARD_NAME);	Serial.print(" with ");
	Serial.println(SHIELD_TYPE);
	Serial.println(PORTENTA_H7_ASYNC_TCP_VERSION);
	Serial.println(PORTENTA_H7_ASYNC_WEBSERVER_VERSION);

	///////////////////////////////////

	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		// don't continue
		while (true);
	}

	Serial.print(F("Connecting to SSID: "));
	Serial.println(ssid);
	status = WiFi.begin(ssid, pass);
	delay(1000);

	// attempt to connect to WiFi network
	while ( status != WL_CONNECTED)
	{
		delay(500);
	// Connect to WPA/WPA2 network
		status = WiFi.status();
	}
	printWifiStatus();

	///////////////////////////////////

	server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)	{	request->send(200, "text/plain", "HomePage");	});
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.print(F("HTTP EthernetWebServer is @ IP : "));
	Serial.println(WiFi.localIP());
}


void loop()
{

}
