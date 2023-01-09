#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include "max6675.h"
#include <WiFiUdp.h>
#include <stdio.h>
#include "time.h"

const char* ssid = "wapp";
const char* password = "qyNy4uhAxy";

const char* mqtt_server = "104.211.178.203";
const char *topic = "PlantData";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)

String formattedDate;
char str_turb[6];
float t = 0;
int min1;
int hour1;
int day1;
int month1;
int year1;

long current;
struct tm timeinfo;

int thermoDO = 19;
int thermoCS = 23;
int thermoCLK = 5;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP; // UDP client
NTPClient timeClient(ntpUDP); // NTP client

long lastMsg = 0;
char msg[50];

void setup() {
  //Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  // publish and subscribe
 client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
 /*Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");*/
 for (int i = 0; i < length; i++) {
     //Serial.print((char) payload[i]);
 }
 //Serial.println();
 //Serial.println("-----------------------");
}

void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    //Serial.println("Failed to obtain time");
    return;
  }else{
  //Serial.println(&timeinfo, "%d-%m-%YT%H-%M");
  
  }
  delay(2000);
}

void loop() {

    t = thermocouple.readCelsius();
    Serial.print("C = ");
    Serial.println(thermocouple.readCelsius());
 
    delay(100);

printLocalTime();
   if(!getLocalTime(&timeinfo)){
    //Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%d-%m-%YT%H-%M");
   int day1 = timeinfo.tm_mday;
  int month1 = timeinfo.tm_mon+1;
  int year1 = timeinfo.tm_year+1900;
  int hour1 = timeinfo.tm_hour;
  int min1 = timeinfo.tm_min;

String s = "";
s = s + day1 + "-" + month1 + "-" + String(year1) + "T" + String(hour1) + "-" + String(min1);

char s2[50];
   int returnValue, s2_size = 50;
    strcpy(s2,s.c_str());

  delay(1000);
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  while(!timeClient.update()) {
  timeClient.forceUpdate();
}

  long now = millis();
  if (now - lastMsg > 20000){
    lastMsg = now;

   char arr[200];
   int returnValue, arr_size = 100;

   snprintf(arr, arr_size, "plantid:AirMassFlowMeter,AIR_FLOW:%f,TIMESTAMP:%s", t, s2);
   //Serial.println(arr);
   client.publish(topic, arr);

}
delay(1000);
}

void setup_wifi() {
  // We start by connecting to a WiFi network
  /*Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);*/

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  /*Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      //Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      /*Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");*/
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
