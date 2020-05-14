#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// Constants
const char* ssid = "Hooper_v1";
const char* password = "qwert123";
const char *host = "192.168.4.1";

HTTPClient http;    //Declare object of class HTTPClient
String  station, getData;
String Link10 = "http://192.168.4.1/station10";
String Link5 = "http://192.168.4.1/station5";
int httpGETReq;
String GOT_str;

int LDR_value, LDR_base_value = 0, LDR_threshold = 500;
int LDR_value2, LDR_base_value2 = 0, LDR_threshold2 = 500;
int selectPIN1 = D4, selectPIN2 = D3, selectPIN3 = D2;

int max_score = 0, score = 0;

int last_TT = millis(), triggered = 0;

//=======================================================================
//                    Selecting PIN of MUX
//=======================================================================
int get_mux(int pin)
{
  digitalWrite (selectPIN1, (pin & 1) ? HIGH : LOW);  // low-order bit
  digitalWrite (selectPIN2, (pin & 2) ? HIGH : LOW);
  digitalWrite (selectPIN3, (pin & 4) ? HIGH : LOW);  // high-order bit
  return analogRead(A0);
}

//=======================================================================
//                    Adjusting LDR base value
//=======================================================================
void adjust_LDR()
{ 
  Serial.println("Calibrating LDR_1...");
  int local_A0, i, N = 128;
  LDR_base_value = 0;
  for(i=0; i<N;i++)
  {
    local_A0 = get_mux(1);
    LDR_base_value += local_A0;
    delay(1);
  }
  LDR_base_value = LDR_base_value / N ;
  Serial.print("LDR calibrated for base value: ");
  Serial.println(LDR_base_value);
  LDR_threshold = LDR_base_value * 0.9;

  Serial.println("Calibrating LDR_2...");
  local_A0 = 0;
  LDR_base_value2 = 0;
  for(i=0; i<N;i++)
  {
    local_A0 = get_mux(3);
    LDR_base_value2 += local_A0;
    delay(1);
  }
  LDR_base_value2 = LDR_base_value2 / N ;
  Serial.print("LDR_2 calibrated for base value: ");
  Serial.println(LDR_base_value2);
  LDR_threshold2 = LDR_base_value2 * 0.9;
  
}
//=======================================================================
//                    Make GET
//=======================================================================
void make_get(int value)
{
  if (value == 10)  http.begin(Link10);     //Specify request destination
  if (value == 5)   http.begin(Link5);
  else http.begin(Link10);
  
  httpGETReq = http.GET();            //Send the request
  GOT_str = http.getString();    //Get the response payload

  Serial.println(httpGETReq);   //Print HTTP return code, 200 normally
  Serial.println(GOT_str);    //Print request response payload

  http.end();  //Close connection
}

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
   
  pinMode(selectPIN1, OUTPUT);
  pinMode(selectPIN2, OUTPUT);
  pinMode(selectPIN3, OUTPUT);
  delay(3000);
  adjust_LDR();
  delay(3000);
  /*
  WiFi.mode(WIFI_OFF);        
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     
  Serial.println("");

  Serial.print("Connecting");  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
*/
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {



  LDR_value = get_mux(1);
  LDR_value2 = get_mux(3);
  if ( (LDR_value < LDR_threshold) && (millis() - last_TT > 1000))
  {
    last_TT = millis();
    triggered = 255;
    make_get(10);  // sending a GET to the server to make a score
  }


  
  if ( (LDR_value2 < LDR_threshold2) && (millis() - last_TT > 1000))
  {
    last_TT = millis();
    triggered = 255;
    make_get(5);  // sending a GET to the server to make a score
  }
  
  //if ( (LDR_value > LDR_threshold) && (millis() - last_TT > 1000))
  //{    triggered = 0;  }
  
  Serial.print(0);  Serial.print("\t");
  Serial.print(1023);  Serial.print("\t");
  Serial.print(triggered); Serial.print("\t");
  Serial.print(LDR_value); Serial.print("\t");
  Serial.print(LDR_value2); Serial.print("\t");
  Serial.print(LDR_threshold); Serial.print("\t");
  Serial.print(LDR_threshold2); Serial.print("\n");
  triggered = 0;
  //delay(5000);  //GET Data at every 5 seconds
}
//=======================================================================
