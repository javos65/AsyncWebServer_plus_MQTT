/****************************************************************************************************************************
Example of Portenta MQTT plus Async Webserver crashing in 1-2 minutes after html web-all

Async Libraries examples using:

https://github.com/khoih-prog/AsyncMQTT_Generic
https://github.com/khoih-prog/Portenta_H7_AsyncWebServer

The examples straight from the Libararies examples, combined in one Sketch
MQTT socket setup and data send
Then web-server calls make this crash

Chunked version : drawGraph(AsyncWebServerRequest *request)  function with mmor reserve
 *****************************************************************************************************************************/
/*
  This example uses FreeRTOS softwaretimers as there is no built-in Ticker library
*/

#include "defines.h"

#include <AsyncMqtt_Generic.h>
#include <Portenta_H7_AsyncWebServer.h>

using namespace rtos;
Thread connectThread;

//#define MQTT_HOST         IPAddress(192, 168, 2, 110)
#define MQTT_HOST         "192.168.200.20"        // Broker address
#define MQTT_PORT         1883

const char *PubTopic  = "async-mqtt/Portenta_H7_WiFi_Pub";               // Topic to publish

AsyncMqttClient mqttClient;

bool connectedWiFi  = false;
bool connectedMQTT  = false;

// In bytes
#define STRING_SIZE                    40000
AsyncWebServer    server(80);

int reqCount = 0;                // number of requests received

#define LED_OFF             HIGH
#define LED_ON              LOW


#define BUFFER_SIZE         1024
char temp[BUFFER_SIZE];

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
<img src=\"/test.svg\" />\
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

String out;

void drawGraph(AsyncWebServerRequest *request)
{
  //out.reserve(STRING_SIZE);
  char temp[70];

  digitalWrite(LED_BUILTIN, LED_ON);

  out = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1810\" height=\"150\">\n";
  out += "<rect width=\"1810\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"2\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"blue\">\n";
  int y = rand() % 130;

  for (int x = 10; x < 5000; x += 10)
  {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"2\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }

  out += "</g>\n</svg>\n";

  AWS_LOGDEBUG1("Total length to send in chunks =", out.length());

  AsyncWebServerResponse *response = request->beginChunkedResponse("image/svg+xml", [](uint8_t *buffer, size_t maxLen, size_t filledLength) -> size_t
  {
    size_t len = min(maxLen, out.length() - filledLength);
    memcpy(buffer, out.c_str() + filledLength, len);

    AWS_LOGDEBUG1("Bytes sent in chunk =", len);
    
    return len;
  });

  request->send(response);

  digitalWrite(LED_BUILTIN, LED_OFF);
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

#define MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP       5
bool connectToWifi()
{
uint8_t numWiFiConnectTries = 0;
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  Serial.print(F("Connecting to SSID: "));
  Serial.println(WIFI_SSID);
  // attempt to connect to WiFi network
  while ( (status != WL_CONNECTED) && (numWiFiConnectTries++ < MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP) )
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(500);
  }
  if (status != WL_CONNECTED)
  {
    // Restart for Portenta as something is very wrong
    Serial.println("Resetting. Can't connect to any WiFi");
    NVIC_SystemReset();
  }
  printWifiStatus();
  connectedWiFi = (status == WL_CONNECTED);
  return (status == WL_CONNECTED);
}

// Check connection every 1s
#define MQTT_CHECK_INTERVAL_MS     1000
void connectToMqttLoop()
{
  while (true)
  {
    if ( (WiFi.status() == WL_CONNECTED) && (WiFi.RSSI() != 0) )      // temporary workaround
    {
      if (!connectedMQTT)  {  mqttClient.connect(); connectedMQTT = true ; }
      if (!connectedWiFi)  {  Serial.println("WiFi reconnected"); connectedWiFi = false; }
    }
    else
    {
      if (!connectedWiFi)
      {
        Serial.println("WiFi disconnected. Reconnecting");
        connectedWiFi = false;
        connectedMQTT = false;
        connectToWifi();
      }
    }
    delay(MQTT_CHECK_INTERVAL_MS);
  }
}

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void printSeparationLine()
{
  Serial.println("************************************************");
}

void onMqttConnect(bool sessionPresent)
{
  Serial.print("Connected to MQTT broker: ");
  Serial.print(MQTT_HOST);
  Serial.print(", port: ");
  Serial.println(MQTT_PORT);
  Serial.print("PubTopic: ");
  Serial.println(PubTopic);

  printSeparationLine();
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  uint16_t packetIdSub = mqttClient.subscribe(PubTopic, 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);

  mqttClient.publish(PubTopic, 0, true, "Portenta_H7_WiFi Test1");
  Serial.println("Publishing at QoS 0");

  uint16_t packetIdPub1 = mqttClient.publish(PubTopic, 1, true, "Portenta_H7_WiFi Test 2");
  Serial.print("Publishing at QoS 1, packetId: ");
  Serial.println(packetIdPub1);

  uint16_t packetIdPub2 = mqttClient.publish(PubTopic, 2, true, "Portenta_H7_WiFi Test 3");
  Serial.print("Publishing at QoS 2, packetId: ");
  Serial.println(packetIdPub2);
  printSeparationLine();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void) reason;
  connectedMQTT = false;
  Serial.println("Disconnected from MQTT.");
}

void onMqttSubscribe(const uint16_t& packetId, const uint8_t& qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(const uint16_t& packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties,
                   const size_t& len, const size_t& index, const size_t& total)
{
  char message[len + 1];

  memcpy(message, payload, len);
  message[len] = 0;

  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  message: ");
  Serial.println(message);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(const uint16_t& packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup()
{
  out.reserve(STRING_SIZE);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_OFF);
  
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  Serial.print("\nStarting FullyFeatured_PortentaH7_WiFi on ");
  Serial.println(BOARD_NAME);
  Serial.println(ASYNC_MQTT_GENERIC_VERSION);

  ///////////////////////////////////
  connectToWifi();
  ///////////////////////////////////
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)  {   handleRoot(request);  });
  server.on("/test.svg", HTTP_GET, [](AsyncWebServerRequest * request)  {    drawGraph(request);  });
  server.on("/inline", [](AsyncWebServerRequest * request)  {    request->send(200, "text/plain", "This works as well");  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print(F("HTTP EthernetWebServer is @ IP : "));
  Serial.println(WiFi.localIP());

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  // Add "connectToMqttLoop" loop to control connection To Mqtt
  connectThread.start(connectToMqttLoop);
  connectToMqtt();
  
}

long int timer=millis();
void loop()
{

}
