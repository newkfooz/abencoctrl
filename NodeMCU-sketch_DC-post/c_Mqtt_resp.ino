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
