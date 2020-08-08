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
