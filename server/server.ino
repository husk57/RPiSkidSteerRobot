//#include <dummy.h>
#include <WiFi.h>

const char* ssid     = "DO NOT CONNECT TO";
const char* password = "boobies!";

//on port 8080
WiFiServer server(8080);

String header;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
  server.begin();
  pinMode(15, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  WiFiClient client = server.available(); 
  if (client) {
    Serial.println("New Client.");  
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.println(c);
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
      }
    }
  }
  digitalWrite(15, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);     
  digitalWrite(15, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
