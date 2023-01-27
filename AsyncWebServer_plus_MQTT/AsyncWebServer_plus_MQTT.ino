/****************************************************************************************************************************
  Basic MQTT example (without SSL!)
  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server then:
  - publishes {Hello from MQTTClient_SSL on NUCLEO_F767ZI} to the topic [STM32_Pub]
  - subscribes to the topic [STM32_Sub], printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

*/

// To remove boolean warnings caused by PubSubClient library
#define boolean     bool

#include "defines.h"
#include <WiFi.h>
#include <Portenta_H7_AsyncWebServer.h>
#include <PubSubClient.h>
int status = WL_IDLE_STATUS;

//const char my_cert[]  = "FIXME";
//const char my_key[]   = "FIXME";

	const char* MQTT_SERVER = "192.168.200.20";        // Broker address
	const char*  ID         = "MyPortenta7";  // Name of our device, must be unique
	String      topic       = "Portenta_H7/Pub";              // Topic to subcribe to
	String      subTopic    = "Portenta_H7/Sub";              // Topic to subcribe to
void mqtt_receive_callback(char* topic, byte* payload, unsigned int length);
  const int   MQTT_PORT           = 1883; //if you use SSL //1883 no SSL
  unsigned long lastMsg = 0;

// Initialize the SSL client library
// Arguments: EthernetClient, our trust anchors
WiFiClient      wifiClient;
PubSubClient    client(MQTT_SERVER, MQTT_PORT, mqtt_receive_callback, wifiClient);
String data         = "Hello from MQTT_ThingStream on " + String(BOARD_NAME) + " with " + String(SHIELD_TYPE);
const char *pubData = data.c_str();

AsyncWebServer    server(80);
int reqCount = 0;                // number of requests received
#define LED_OFF             HIGH
#define LED_ON              LOW
#define BUFFER_SIZE         512
char temp[BUFFER_SIZE];

/*
   Called whenever a payload is received from a subscribed MQTT topic
*/
void mqtt_receive_callback(char* topic, byte* payload, unsigned int length)
{
	Serial.print("MQTT Message receive [");
	Serial.print(topic);
	Serial.print("] ");
	for (unsigned int i = 0; i < length; i++)	{ Serial.print((char)payload[i]);	}
	Serial.println();
}

void reconnect()
{
	// Loop until we're reconnected
	while (!client.connected())
	{
		Serial.print("Attempting MQTT connection to ");
		Serial.println(MQTT_SERVER);
		// Attempt to connect
		int connect_status = client.connect(ID);
		if (connect_status)
		{
			Serial.println("...connected");
			// Once connected, publish an announcement...
			client.publish(topic.c_str(), data.c_str());
			Serial.println("Published connection message successfully!");
			Serial.print("Subcribed to: ");	Serial.println(subTopic);

			// This is a workaround to address https://github.com/OPEnSLab-OSU/SSLClient/issues/9
			//ethClientSSL.flush();
			// ... and resubscribe
			client.subscribe(subTopic.c_str());
			// for loopback testing
			//client.subscribe(topic.c_str());
			// This is a workaround to address https://github.com/OPEnSLab-OSU/SSLClient/issues/9
			//ethClientSSL.flush();
		}
		else
		{
			Serial.print("*! Mqtt post failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");

			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("Local IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}



void handleRoot(AsyncWebServerRequest *request)
{
  digitalWrite(LED_BUILTIN, LED_ON);

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>AsyncWebServer-%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h2>AsyncWebServer_Portenta_H7!</h2>\
<h3>running WiFi on %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
</body>\
</html>", BOARD_NAME, BOARD_NAME, day, hr % 24, min % 60, sec % 60);

  request->send(200, "text/html", temp);

  digitalWrite(LED_BUILTIN, LED_OFF);
}

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




void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial && millis() < 5000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);
  
	Serial.print("\nStart Async Webserver plus Local MQTT_on ");
	Serial.print(BOARD_NAME);
	Serial.print(" with ");
	Serial.println(SHIELD_TYPE);
	Serial.println(PORTENTA_H7_ASYNC_TCP_VERSION);
	Serial.println(PORTENTA_H7_ASYNC_WEBSERVER_VERSION);
	///////////////////////////////////

	// check for the WiFi module:
	int con=0;
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("*! No WiFi module found");
		while (true);
	}

	Serial.print("Connecting to SSID: ");	Serial.println(ssid);
	status = WiFi.begin(ssid, pass);
	delay(1000);
	// attempt to connect to WiFi network
	while ( status != WL_CONNECTED)
	{
		delay(500);
   if(con++>5) {Serial.println("*! Wifi could not Connect");break;}
		// Connect to WPA/WPA2 network
		status = WiFi.status();
	}
 	printWifiStatus();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)  { handleRoot(request); });
  server.on("/inline", [](AsyncWebServerRequest * request) { request->send(200, "text/plain", "This works as well"); });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print("* HTTP WebServer is @ IP : "); Serial.println(WiFi.localIP());
	///////////////////////////////////

	// Note - the default maximum packet size is 256 bytes. If the
	// combined length of clientId, username and password exceed this use the
	// following to increase the buffer size:
  client.setBufferSize(256);

	Serial.println("***************************************");
	Serial.println(topic);
	Serial.println("***************************************");
}

#define MQTT_PUBLISH_INTERVAL_MS      20000L

void loop()
{
	static unsigned long now;
	if (!client.connected())	{		reconnect();	}

	// Sending Data
	now = millis();
	if (now - lastMsg > MQTT_PUBLISH_INTERVAL_MS)
	{
		lastMsg = now;
		if (!client.publish(topic.c_str(), pubData))	{	Serial.println("*! Message failed to send.");	}
		Serial.print("* MQTT Message Send : " + topic + " => ");
		Serial.println(data);
	}
	client.loop();

}
