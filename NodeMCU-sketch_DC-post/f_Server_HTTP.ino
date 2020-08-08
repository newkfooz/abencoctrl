////_______ Server HTTP _______///

void runServer(){
 
  Serial.println("new client");                        // Wait until the client sends some data
  digitalWrite(0, HIGH);
  digitalWrite(2, LOW);                                                  
 
  if (server.method() == HTTP_POST) {                  // Match the request:
    
    if (server.arg("clicked")== "on"){
        digitalWrite(pinRelay, HIGH);
        value = HIGH;
    }else if (server.arg("clicked")== "off"){
        digitalWrite(pinRelay, LOW);
        value = LOW;
    }
  }
  
  const char* srv = (value == HIGH)?"On":"Off";        // Status Relay Variable, can be ON or OFF
  String generatePage = "<html>\
    <head>\
      <link href='data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAU7sALDAAABwfAAAoKwAAFxoAAE7i/ACSkpIAP0UAAE9XAAAqLgAAOkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiIiGaIZoiIiIiIZohmiIiIiIhmiGaIiIiIiGaIZoiIiIiIZohmiIiIiAAAAAAAiId3AAAAAAB3d3d1VVVVV3d6qqBQAAAKqqqqoFAAAAqqoREQUAAAAREZmZBQAAAJmZMzMFUAAAMzMzMzBVUAMzMyIiIgAAIiIiREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA+B8AAP//AAA=' rel='icon' type='image/x-icon' />\
      <link href='data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAU7sALDAAABwfAAAoKwAAFxoAAE7i/ACSkpIAP0UAAE9XAAAqLgAAOkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAiIiGaIZoiIiIiIZohmiIiIiIhmiGaIiIiIiGaIZoiIiIiIZohmiIiIiAAAAAAAiId3AAAAAAB3d3d1VVVVV3d6qqBQAAAKqqqqoFAAAAqqoREQUAAAAREZmZBQAAAJmZMzMFUAAAMzMzMzBVUAMzMyIiIgAAIiIiREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA+B8AAP//AAA=' rel='shortcut icon' type='image/x-icon' />\
      <meta charset='utf-8'>\
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
      <meta name='apple-mobile-web-app-capable' content='yes'>\
      <meta name='mobile-web-app-capable' content='yes'>\
      <title>Solenoid LED control</title>\
      <style>\
        body {background-color:black; height: -webkit-fill-available; font-family: -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, Oxygen-Sans, Ubuntu, Cantarell, Helvetica Neue, sans-serif;}\
        .title-sol {color: #ff9900; size: 60px; margin-top:10px; margin-left: 10px; margin-right: 10px; margin-bottom: 10px; font-weight: 500; position: relative; text-align: center;}\
        .div-block {background: linear-gradient(to bottom, #000000 0%, #006666 100%); position: fixed; top: 100px; right: 0px; left: 0px; padding: 20px; bottom: 0px;}\
        .paragraph { color: white; size:6px; position: relative; margin-top: 30px; margin-right: 30px;}\
        button:hover, button:active { background-color: #ff9900; color: #f3f5f6;}\
        button { text-decoration: none; cursor: pointer; display: inline-block; background-color: #bb5300; color: #f3f5f6; font-size:30px; position: relative; min-height: 60px; width: 120px; text-align: center; padding: 6px 12px; margin-top: 20px; margin-right:10px; margin-left:10px; border-radius: 8px; font-weight: bold; text-transform: uppercase;}\
      </style>\
    </head>\
    <body  leftmargin='0' topmargin='0'>\
      <h1 class='title-sol'>Solenoid control</h1>\
      <div class='div-block'>\
          <p class='paragraph'>Solenoid is now:&nbsp;"; 
  generatePage += srv;
  generatePage += "</p><br><br>\
          <form action='/' method='post' enctype='application/x-www-form-urlencoded'>\
              <button name='clicked' value='on'>On</button>\
              <button name='clicked' value='off'>Off</button><br />\
          </form>\
      </div>\
    </body>\
  </html>";
  
  server.send(200, "text/html", generatePage);
  
  Serial.print("LED set: ");
  Serial.println(srv);
  Serial.println("Client disonnected");
  Serial.println("");
}
