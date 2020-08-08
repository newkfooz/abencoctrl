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
