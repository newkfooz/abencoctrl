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

  server.on("/", runServer);                          // Handle GET or POST method
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
