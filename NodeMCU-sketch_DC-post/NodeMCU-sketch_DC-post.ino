#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);                    // server on port 80

const char* ssid = "FibraMelita_300012986";     // ssid and password of wifi AP
const char* password = "4C77600902";

const char* mqtt_server = "nash.abinsula.com";  // MQTT-broker ip address (indirizzo del tuo pc se usi mosquitto) 192.168.43.148
const char* clientName = "ESP8266_Ampere";      // name of this client, it will be visible from the broker

const int pinRelay = 13;                        // GPIO13---D7 of NodeMCU

String msgIn;                                   // MQTT message input, from byte payload

int value = LOW;                                // site flag-value for the relay last state (on or off)

float ValADC = 0;                               // variables for electric current reading:
const float diodsTensFall = 1.4;
const float Nspire = 2000;
float Vrms;
float Irms;
float kWatt;

float maxCurrent = 30;                    // settings for Current Alarm
float highCurrent = 20;

unsigned long timer;



////////////////////////////////- SETUP -///////////////////////////////

void setup()
{
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  
  digitalWrite(0, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(pinRelay, LOW);
  
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



////////////////////////////////- LOOP -///////////////////////////////

void loop() {
    
    if (!client.connected()) {
          reconnect();
    }

    server.handleClient();
    client.loop();
    
    if (millis() >= (timer + 10000)){
          readLoopCurrent();
          timer = millis();
    }

    if (timer > millis()){                          // if millis() reset to 0 milliseconds, this will reset timer too
          timer = millis();
    }
  
}
