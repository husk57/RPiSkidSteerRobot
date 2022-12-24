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
unsigned long prev_timestamp = millis();

void tankDrive(double leftSide, double rightSide) {
  leftSide = constrain(leftSide, -1, 1);
  rightSide = constrain(rightSide, -1, 1);

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

  //right side
  if (isCoast == true && abs(rightSide) < coastCutoff) {
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    digitalWrite(12, LOW);
    digitalWrite(14, LOW);
  } else {
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
    if (rightSide > 0) {
      //right side
      ledcWrite(3, int(rightSide*255));
      ledcWrite(2,0);
    } else {
      //left side
      ledcWrite(2, int(abs(rightSide)*255));
      ledcWrite(3,0);
    }
  }
}
double speed = 0.0;
double turn = 0.0;

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
    speed = message.substring(0,spliceIdx).toDouble();
    turn = -message.substring(spliceIdx+1).toDouble();
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
  ledcSetup(0, 1000, 8);
  ledcSetup(1, 1000, 8);
  ledcWrite(0, 0);
  ledcWrite(1, 0);

  //right motor controller
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); //power for esc
  pinMode(12, OUTPUT); //left enable
  pinMode(14, OUTPUT); //right enable
  //default to coast
  digitalWrite(12, LOW);
  digitalWrite(14, LOW);
  ledcAttachPin(27, 2); //left pwm
  ledcAttachPin(26, 3); //right pwm
  ledcSetup(2, 1000, 8);
  ledcSetup(3, 1000, 8);
  ledcWrite(2, 0);
  ledcWrite(3, 0);

  if (isCoast == false) {
    digitalWrite(2, HIGH);
    digitalWrite(18, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(14, HIGH);
  }
}

int sign(double value) {
  if (value > 0) {
    return 1;
  } else if (value < 0) {
    return -1;
  } else {
    return 0;
  }
}

double currentLeftSpeed = 0.0;
double currentRightSpeed = 0.0;
double maxSpeed = 1;
double maxAcceleration = 1.; //per second
void loop() {
  double desiredLeftSpeed = maxSpeed*(speed+turn);
  double desiredRightSpeed = maxSpeed*(speed-turn);
  //invert left side
  desiredLeftSpeed *= -1;

  //cap acceleration
  unsigned long currentTimestamp = millis();
  double timeDelta = (currentTimestamp - prev_timestamp) / 1000.0; // time delta in seconds
  double errLeft = desiredLeftSpeed - currentLeftSpeed;
  double errRight = desiredRightSpeed - currentRightSpeed;
  double maxChange = maxAcceleration * timeDelta;
  if (abs(errLeft) > maxChange) {
    currentLeftSpeed += maxChange * sign(errLeft);
  } else {
    currentLeftSpeed = desiredLeftSpeed;
  }
  if (abs(errRight) > maxChange) {
    currentRightSpeed += maxChange * sign(errRight);
  } else {
    currentRightSpeed = desiredRightSpeed;
  }
  //output  
  tankDrive(desiredLeftSpeed, desiredRightSpeed);
  prev_timestamp = currentTimestamp;
}