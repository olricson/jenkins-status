#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

ESP8266WebServer server(80);   //instantiate server at port 80 (http port)

String page = "";
int LED_SUCCESS = D8;
int LED_UNSTABLE = D5;
int LED_FAIL = D1;

void boot_animation() {
  
  Serial.println("Green");
  digitalWrite(LED_SUCCESS, HIGH);
  delay(1000); 
  digitalWrite(LED_SUCCESS, LOW);
  Serial.println("Yellow");
  digitalWrite(LED_UNSTABLE, HIGH);
  delay(1000);
  digitalWrite(LED_UNSTABLE, LOW);
  Serial.println("Red");
  digitalWrite(LED_FAIL, HIGH);
  delay(1000); 
  digitalWrite(LED_FAIL, LOW);
}

void setup(void){
  //the HTML of the web page
  page = "<h1>Simple NodeMCU Web Server</h1><p><a href=\"LEDOn\"><button>ON</button></a>&nbsp;<a href=\"LEDOff\"><button>OFF</button></a></p>";
  //make the LED pin output and initially turned off
  pinMode(LED_SUCCESS, OUTPUT);
  pinMode(LED_UNSTABLE, OUTPUT);
  pinMode(LED_FAIL, OUTPUT);
  digitalWrite(LED_SUCCESS, LOW);
  digitalWrite(LED_UNSTABLE, LOW);
  digitalWrite(LED_FAIL, LOW);
   
  delay(1000);
  Serial.begin(115200);
  
  boot_animation();
  Serial.println("Wifi: ");
  Serial.print("SSID: ");
  Serial.print(ssid);
  Serial.print("\n");
  Serial.print("PWD: ");
  Serial.print(password);
  Serial.print("\n");
  
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_UNSTABLE, HIGH);
    delay(1000); 
    digitalWrite(LED_UNSTABLE, LOW);
  }
  
  digitalWrite(LED_SUCCESS, HIGH);
  delay(2000);
  
  digitalWrite(LED_SUCCESS, LOW);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", [](){
    server.send(200, "text/html", page);
  });
  server.on("/LEDOn", [](){
    Serial.println("Requested URL: /LEDOn");
    server.send(200, "text/html", page);
    digitalWrite(LED_SUCCESS, HIGH);
    digitalWrite(LED_UNSTABLE, HIGH);
    digitalWrite(LED_FAIL, HIGH);
    delay(1000);
  });
  server.on("/LEDOff", HTTP_GET, [](){
    Serial.println("Requested URL: /LEDOff");
    server.send(200, "text/html", page);
    digitalWrite(LED_SUCCESS, LOW);
    digitalWrite(LED_UNSTABLE, LOW);
    digitalWrite(LED_FAIL, LOW);
    delay(1000); 
  });
  server.on("/jenkins", HTTP_POST, [](){
    Serial.println("Requested URL: /jenkins");
    DynamicJsonBuffer jsonBuffer(4000);
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    root.prettyPrintTo(Serial);
    const char* val = root["build"];
    Serial.println(val);
    const char* st = root["build"]["status"];
    Serial.print("Status: ");
    Serial.println(st);
    if (String(st) == "SUCCESS") {
      digitalWrite(LED_SUCCESS, HIGH);
      digitalWrite(LED_UNSTABLE, LOW);
      digitalWrite(LED_FAIL, LOW);
    } else if (String(st) == "UNSTABLE") {
      digitalWrite(LED_SUCCESS, LOW);
      digitalWrite(LED_UNSTABLE, HIGH);
      digitalWrite(LED_FAIL, LOW);
    } else if (String(st) == "FAILURE") {
      digitalWrite(LED_SUCCESS, LOW);
      digitalWrite(LED_UNSTABLE, LOW);
      digitalWrite(LED_FAIL, HIGH);
    }
    
    server.send(200);
  });
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  server.handleClient();
}
