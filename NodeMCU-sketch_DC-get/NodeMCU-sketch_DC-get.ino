#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "My ASUS";                   // ssid and password of wifi AP
const char* password = "17772hdjd";

const char* mqtt_server = "nash.abinsula.com";  // MQTT-broker ip address (indirizzo del tuo pc se usi mosquitto) 192.168.43.148
const char* clientName = "ESP8266_Ampere";      // name of this client, it will be visible from the broker

const int pinRelay = 13;                        // GPIO13---D7 of NodeMCU

String msgIn;                                   // MQTT message input, from byte payload

WiFiServer server(80);                          // server on port 80
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
  digitalWrite(pinRelay, LOW);
  digitalWrite(0, HIGH);
  digitalWrite(2, HIGH);
  
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


////_____ WIFI setup _____////

void setup_wifi(){
  delay(10);
  
  Serial.println();                                  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);                        // Attempting to connect to the AP
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");                   // Connected

  server.begin();                                     // Start the server
  Serial.println("Server started");
 
  Serial.print("Use this URL to connect: ");          // Print the IP address on serial monitor
  Serial.print("http://");                            // URL IP to be typed in mobile/desktop browser
  Serial.print(WiFi.localIP());
  Serial.println("/");

  digitalWrite(0, LOW);                               // red Led if connected and server working
}

void callback(char* topic, byte* payload, unsigned int length) {
  msgIn = "";
  digitalWrite(2, LOW);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msgIn = msgIn + ((char)payload[i]);
  }
  Serial.println(msgIn);
  delay(100);
  ControlsByMessage((String)topic, msgIn);
}


////_____ MQTT message Request control _____////

void ControlsByMessage(String tpc, String msg){
    
  Serial.println(tpc);
  Serial.println(msg);
    
  if (tpc == "/ABI_TIR/REQ"){ 

    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, msg); 
    if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.c_str());
    }
    
    long int reqId = doc["REQUEST"]["request_id"];
    String reqType = doc["REQUEST"]["action_type"];
    String reqDetail = doc["REQUEST"]["action_detail"];
    String reqValue = doc["REQUEST"]["action_value"];
    
    //#### Set relay status ####//
    if (reqType == "SET"){
        if (reqDetail == "relay" && reqValue == "ON"){              // set relay ON
                digitalWrite(pinRelay, HIGH);
                value = HIGH;
                Serial.println("Set relay ON");
                String respStatus = "OK";
                genResponse(reqId, reqValue, respStatus);
                   
        }else if (reqDetail == "relay" && reqValue == "OFF"){       // set relay OFF
                digitalWrite(pinRelay, LOW);
                value = LOW;
                Serial.println("Set relay OFF");
                String respStatus = "OK";
                genResponse(reqId, reqValue, respStatus);
                
        }else if (reqDetail == "shutdown"){                         // set max current value
                maxCurrent = reqValue.toFloat();                    // is necessary a cast to convert a String to a float
                Serial.print("Set max_current to ");
                Serial.println(maxCurrent);
                String respStatus = "OK";
                sendMaxHighCurrent(reqId, maxCurrent, respStatus);
                
        }else if (reqDetail == "high_current"){                     // set high current value
                highCurrent = reqValue.toFloat();                   // is necessary a cast to convert a String to a float
                Serial.print("Set high_current to ");
                Serial.println(highCurrent);
                String respStatus = "OK";
                sendMaxHighCurrent(reqId, highCurrent, respStatus);
                
        }else{
                String errorValue = "ERROR";
                String respStatus = "NOT_OK";
                genResponse(reqId, errorValue, respStatus);
        }
    }

    //#### Info request ####//
    else if (reqType == "GET"){
        if (reqDetail == "relay" && reqValue == "STATUS"){                          // relay status
                String statusRelay = (digitalRead(pinRelay)==1)?"ON":"OFF";
                Serial.println("The Relay is "+statusRelay);
                String respStatus = "OK";
                genResponse(reqId, statusRelay, respStatus);
                
        }else if (reqDetail == "current" && reqValue == "STATUS"){                  // current and power consumption status
                getCurrent();
                sendCurrent(reqId);

        }else if (reqDetail == "shutdown" && reqValue == "STATUS"){                 // max current status
                Serial.print("Max_current is set to ");
                Serial.println(maxCurrent);
                String respStatus = "OK";
                sendMaxHighCurrent(reqId, maxCurrent, respStatus);

        }else if (reqDetail == "high_current" && reqValue == "STATUS"){             // high current status
                Serial.print("High_current is set to ");
                Serial.println(highCurrent);
                String respStatus = "OK";
                sendMaxHighCurrent(reqId, highCurrent, respStatus);
        
        }else{
                String errorValue = "ERROR";
                String respStatus = "NOT_OK";
                genResponse(reqId, errorValue, respStatus);
        }
    }
  }else{}
}


////_____ Generate a Response MQTT _____////

void genResponse(long int reqId, String returnValue, String respStatus){
      StaticJsonDocument<200> doc;
      JsonObject RESPONSE = doc.createNestedObject("RESPONSE");   // create a nested object
      RESPONSE["request_id"] = reqId;
      RESPONSE["return_value"] = returnValue;
      RESPONSE["return_status"] = respStatus;
      char resp_buff[200];                                        // char buffer for upload the response Json to Mqtt pub
      serializeJson(doc, resp_buff);
      client.publish("/ABI_TIR/RES",resp_buff);
}


////_____ Send HIGH or MAX current values by MQTT _____////

void sendMaxHighCurrent(long int reqId, int returnValue, String respStatus){
      StaticJsonDocument<200> doc;
      JsonObject RESPONSE = doc.createNestedObject("RESPONSE");   // create a nested object
      RESPONSE["request_id"] = reqId;
      RESPONSE["return_value"] = returnValue;
      RESPONSE["return_status"] = respStatus;
      char resp_buff[200];                                        // char buffer for upload the response Json to Mqtt pub
      serializeJson(doc, resp_buff);
      client.publish("/ABI_TIR/RES",resp_buff);
}


////_____ Send Current value by MQTT _____////

void sendCurrent(long int reqId){
      StaticJsonDocument<200> doc;
      JsonObject RESPONSE = doc.createNestedObject("RESPONSE");     // create a nested object
      RESPONSE["request_id"] = reqId;
      RESPONSE["current_value"] = Irms;
      RESPONSE["power_value"] = kWatt;
      RESPONSE["return_status"] = "OK";
      
      char curr_buff[200];                                        // char buffer for upload the response Json to Mqtt pub
      serializeJson(doc, curr_buff);
      client.publish("/ABI_TIR/RES",curr_buff);      
}


////_______ Loop read and control Current value and call EVENT MQTT _______///

void readLoopCurrent(){
      getCurrent();
      if (Irms >= highCurrent && Irms < maxCurrent){
            Serial.println("NOTIFY");
            String type = "NOTIFY";
            String eventValue = "high_current";
            genAlarm(type, eventValue);
      }else if(Irms >= maxCurrent){
            Serial.println("ALARM");
            String type = "ALARM";
            String eventValue = "max_current";
            genAlarm(type, eventValue);
      }else{
      }
}


////_____ Reading Current value from A0 _____////

void getCurrent(){

      Irms = 0;
      kWatt = 0;
      ValADC = analogRead(A0);

      Vrms = (ValADC/1024)*diodsTensFall;
      Irms = (Vrms/20)*2000;
      kWatt = Irms*230/1000;
      
      Irms = round(Irms*1000)/1000;                                 // Current round to the third decimal digit
      kWatt = round(kWatt*1000)/1000;                               // Power round to the third decimal digit
      
      Serial.println("");
      Serial.print(ValADC);
      Serial.println(" on A0 pin;");
      Serial.print(Vrms);
      Serial.print(" V;  ");
      Serial.print(Irms, 3);
      Serial.print(" A;  ");
      Serial.print(kWatt, 3);                                       // Serial.print rounded float to 3 decimal digits
      Serial.println(" kW;");
      Serial.println("");
}


////_____ Generate an Alarm MQTT _____////

void genAlarm(String type, String eventValue){
      StaticJsonDocument<200> doc;
      JsonObject EVENTS = doc.createNestedObject("EVENTS");   // create a nested object
      EVENTS["event_type"] = type;
      EVENTS["notify_value"] = Irms;
      EVENTS["alarm_value"] = eventValue;
      char alarm_buff[200];                                        // char buffer for upload the response Json to Mqtt pub
      serializeJson(doc, alarm_buff);
      client.publish("/ABI_TIR/EVENTS",alarm_buff);              // in a different topic: /EVENTS
}


////______ reconnect to MQTT broker _____////

void reconnect() {
  while (!client.connected()) {                          // If MQTT connection lost
    digitalWrite(0, HIGH);                               // Loop until we're reconnected
    digitalWrite(2, HIGH);
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect(clientName)) {                    // Attempt to connect
      digitalWrite(0, LOW);                              // red Led if connected and server working
      Serial.println("connected");
      client.subscribe("/ABI_TIR/REQ");             // subscription to Topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);                                       // Wait 5 seconds before retrying
    }
  }
}


////_______ Server HTTP _______///

void runServer(){
      WiFiClient client = server.available();            // Check if a client has connected
      if (!client) {
        return;
      }
 
      Serial.println("new client");                      // Wait until the client sends some data
      digitalWrite(0, HIGH);
      digitalWrite(2, LOW);

      String request = client.readStringUntil('\r');     // Read the first line of the request
      Serial.println(request);
      client.flush();
                                                         // Match the request:
      if (request.indexOf("/Solenoid=ON") != -1)  {
        digitalWrite(pinRelay, HIGH);
        value = HIGH;
      }
      if (request.indexOf("/Solenoid=OFF") != -1)  {
        digitalWrite(pinRelay, LOW);
        value = LOW;
      }
 
                                                          // Return the response
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");                                 //  do not forget this one
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
  
      client.println("<head>");
      client.println("<link href='data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAU7sALDAAABwfAAAoKwAAFxoAAE7i/ACSkpIAP0UAAE9XAAAqLgAAOkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiIiGaIZoiIiIiIZohmiIiIiIhmiGaIiIiIiGaIZoiIiIiIZohmiIiIiAAAAAAAiId3AAAAAAB3d3d1VVVVV3d6qqBQAAAKqqqqoFAAAAqqoREQUAAAAREZmZBQAAAJmZMzMFUAAAMzMzMzBVUAMzMyIiIgAAIiIiREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA+B8AAP//AAA=' rel='icon' type='image/x-icon' />");
      client.println("<link href='data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAU7sALDAAABwfAAAoKwAAFxoAAE7i/ACSkpIAP0UAAE9XAAAqLgAAOkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiIiGaIZoiIiIiIZohmiIiIiIhmiGaIiIiIiGaIZoiIiIiIZohmiIiIiAAAAAAAiId3AAAAAAB3d3d1VVVVV3d6qqBQAAAKqqqqoFAAAAqqoREQUAAAAREZmZBQAAAJmZMzMFUAAAMzMzMzBVUAMzMyIiIgAAIiIiREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA+B8AAP//AAA=' rel='shortcut icon' type='image/x-icon' />");
      client.println("<meta charset='utf-8'>");
      client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
      client.println("<meta name='apple-mobile-web-app-capable' content='yes'>");
      client.println("<meta name='mobile-web-app-capable' content='yes'>");
      client.println("<title>Solenoid LED control</title>");
      client.println("</head>");
  
      client.println("<body  leftmargin='0' topmargin='0' style='background-color:black; height: -webkit-fill-available; font-family: -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, Oxygen-Sans, Ubuntu, Cantarell, Helvetica Neue, sans-serif;'>");
      client.println("<h1 style='color: #ff9900; size: 60px; margin-top:10px; margin-left: 10px; margin-right: 10px; margin-bottom: 10px; font-weight: 500; position: relative; text-align: center;'>Solenoid control</h1>");
      client.println("<div style='background: linear-gradient(to bottom, #000000 0%, #006666 100%); position: fixed; position: fixed; top: 100px; right: 0px; left: 0px; padding: 20px; bottom: 0px;'>");
      client.print("<p style='color: white; size:6px; position: relative; margin-top: 30px; margin-right: 30px;'>Solenoid is now:&nbsp;");
 
      if(value == HIGH) {
        client.print("On");
      } else {
        client.print("Off");
      }
  
      client.println("</p>");
      client.println("<br><br>");
      client.println("<button onclick='powerOn()'>On </button>");
      client.println("<button onclick='powerOff()'>Off </button><br />");
      client.println("</div>");
      client.println("</body>"); 
  
      client.println("<script>");
      client.println("function powerOn(){ window.location.href = '/Solenoid=ON'; }");
      client.println("function powerOff(){ window.location.href = '/Solenoid=OFF'; }");
      client.println("</script>");
   
      client.println("<style>");
      client.println("button:hover, button:active { background-color: #ff9900; color: #f3f5f6;}");
      client.println("button { text-decoration: none; cursor: pointer; display: inline-block; background-color: #bb5300; color: #f3f5f6; font-size:30px; position: relative; min-height: 60px; width: 120px; text-align: center; padding: 6px 12px; margin-top: 20px; margin-right:10px; margin-left:10px; border-radius: 8px; font-weight: bold; text-transform: uppercase;}");
      client.println("</style>");
      client.println("</html>");
  
      //delay(10);
      Serial.println("Client disonnected");
      Serial.println("");
}



////////////////////////////////- LOOP -///////////////////////////////

void loop() {
    
    if (!client.connected()) {
          reconnect();
    }
    runServer();
    client.loop();
    
    if (millis() >= (timer + 10000)){
          readLoopCurrent();
          timer = millis();
    }

    if (timer > millis()){                          // if millis() reset to 0 milliseconds, this will reset timer too
          timer = millis();
    }
  
}
