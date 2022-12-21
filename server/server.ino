//#include <dummy.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncWebSocket.h>

const char* ssid     = "DO NOT CONNECT TO";
const char* password = "boobies!";

//on port 8080
AsyncWebServer server(8080);
AsyncWebSocket ws("/ws");
bool isCoast = false;
double coastCutoff = 0.1;

void tankDrive(double leftSide, double rightSide) {
  //left side
  if (isCoast == true && abs(leftSide) < coastCutoff) {
    ledcWrite(0,0);
    ledcWrite(1,0);
    digitalWrite(2, LOW);
    digitalWrite(18, LOW);
  } else {
    digitalWrite(2, HIGH);
    digitalWrite(18, HIGH);
    if (leftSide > 0) {
      //right side
      ledcWrite(1, int(leftSide*255));
      ledcWrite(0,0);
    } else {
      //left side
      ledcWrite(0, int(abs(leftSide)*255));
      ledcWrite(1,0);
    }
  }
}
void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.println("Websocket client connection received");
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");
  } else if(type == WS_EVT_DATA){
    String message = "";
    for(size_t i=0; i<len; i++) {
      message += (char) data[i];
    }

    int spliceIdx = message.indexOf("$");
    double speed = message.substring(0,spliceIdx).toDouble();
    double turn = message.substring(spliceIdx+1).toDouble();

    tankDrive(speed, 0.0);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  server.begin();

  //left motor controller

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH); //power for esc
  pinMode(2, OUTPUT); //left enable
  pinMode(18, OUTPUT); //right enable
  //default to coast
  digitalWrite(2, LOW);
  digitalWrite(18, LOW);
  ledcAttachPin(4, 0); //left pwm
  ledcAttachPin(5, 1); //right pwm
  ledcSetup(0, 100, 8);
  ledcSetup(1, 100, 8);
  ledcWrite(0, 0);
  ledcWrite(1, 0);

  if (isCoast == false) {
    digitalWrite(2, HIGH);
    digitalWrite(18, HIGH);
  }
}

// the loop function runs over and over again forever
void loop() {

}
