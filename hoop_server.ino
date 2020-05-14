/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>

const char* ssid     = "Hooper_v1";
const char* password = "qwert123";

int player_score = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Hoop v1.0 Server</h2>
  <p>
    <span class="dht-labels">Point</span> 
    <span id="score">%SCORE%</span>
  </p>
  <p>
    <span class="dht-labels">Record</span>
    <span id="record">%RECORD%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("score").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/score", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("record").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/record", true);
  xhttp.send();
}, 10000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "SCORE"){
    return String(player_score);
  }
  else if(var == "RECORD"){
    return String(120);
  }
  else if(var == "STATION10"){
    return String(player_score);
  }
  else if(var == "STATION5"){
    return String(player_score);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/score", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(player_score).c_str());
  });
  server.on("/record", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(120).c_str());
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String("RESET").c_str());
    player_score = 0;
  });
  server.on("/station10", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print("Got GET ->");
    Serial.println(player_score);
    player_score += 10;
    request->send_P(200, "text/plain", String(player_score).c_str());
  });
  server.on("/station5", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.print("Got GET ->");
    Serial.println(player_score);
    player_score += 5;
    request->send_P(200, "text/plain", String(player_score).c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    
      //Serial.println();
    }
}

