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
