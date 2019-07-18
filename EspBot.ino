#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

#define WIFI_ENABLED 1

#ifdef WIFI_ENABLED
  #include <ESP8266WebServer.h>
  // Set these if wifi enabled
  #define WLAN_SSID ""
  #define WLAN_PASS ""
  #define HOSTNAME "EspBot-NodeMcu-"
  ESP8266WebServer server(80); 
#endif

#define PIN            1
#define NUMPIXELS      1

#define MotorPWMPin1   15
#define MotorDirPin1   13

#define MotorPWMPin2   14
#define MotorDirPin2   12

// from -1023 to 1023
#define MinSpeed          -1023
#define MaxSpeed          1023
#define SpeedBack         -1023
#define SpeedForward      1023
#define SpeedStopped      0
#define SpeedAdjustRight  1.0
#define SpeedAdjustLeft   1.0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t blue = pixels.Color(0, 0, 255);
uint32_t white = pixels.Color(255, 255, 255, 255);

enum directions {
  FORWARD,
  BACK
};

enum motor {
  RIGHT_MOTOR,
  LEFT_MOTOR
};

// defines pins numbers
const int trigPin = D2; 
const int echoPin = D1; 

const int trigPin2 = D4;
const int echoPin2 = D3;

long durationFront;
long durationSide;

int distanceFront;
int distanceSide;

bool OTAStarted;
bool stopped;
bool shouldTurnLeft;
bool shouldTurnRight;

bool shouldGoForward;
bool shouldGoBackward;

void setupOta() {
  ArduinoOTA.setPort(8266);

  ArduinoOTA.begin();
  ArduinoOTA.onStart(onOTAStart);
}

void onOTAStart() {
  OTAStarted = true;
}

#ifdef WIFI_ENABLED
  void handleRoot() { 
    server.send(200, "text/html", "<form action=\"/toggle-motors\" method=\"POST\"><input type=\"submit\" value=\"Toggle motors on/off\"></form><br><form action=\"/turn-left\" method=\"POST\"><input type=\"submit\" value=\"Turn left\"></form><br><form action=\"/turn-right\" method=\"POST\"><input type=\"submit\" value=\"Turn right\"></form><br><form action=\"/go-forward\" method=\"POST\"><input type=\"submit\" value=\"Go forward\"></form><br><form action=\"/go-backward\" method=\"POST\"><input type=\"submit\" value=\"Go backward\"></form>");
  }

  void goForwardHandler() {                          
    shouldGoForward = true;
    server.sendHeader("Location","/");
    server.send(303); 
  }

  void goBackwardHandler() {                          
    shouldGoBackward = true;
    server.sendHeader("Location","/");
    server.send(303); 
  }

  void turnLeftHandler() {                          
    shouldTurnLeft = true;
    server.sendHeader("Location","/");
    server.send(303); 
  }

  void turnRightHandler() {                          
    shouldTurnRight = true;
    server.sendHeader("Location","/");
    server.send(303); 
  }
  
  void toggleMotors() {                          
    stopped = stopped != true;
    server.sendHeader("Location","/");
    server.send(303); 
  }
  
  void handleNotFound(){
    server.send(404, "text/plain", "404: Not found");
  }
  
  void setupWifi() 
  {
    WiFi.mode(WIFI_STA);
    // Set Hostname.
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    WiFi.hostname(hostname);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
    }
    // http server handlers
    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle-motors", HTTP_POST, toggleMotors);
    server.on("/turn-left", HTTP_POST, turnLeftHandler);
    server.on("/turn-right", HTTP_POST, turnRightHandler);
    server.on("/go-forward", HTTP_POST, goForwardHandler);
    server.on("/go-backward", HTTP_POST, goBackwardHandler);
    server.onNotFound(handleNotFound);

    server.begin();
  }
#endif

void setupMotors()
{
  digitalWrite(MotorPWMPin1, LOW);
  pinMode(MotorPWMPin1, OUTPUT);
  digitalWrite(MotorPWMPin1, LOW);
  digitalWrite(MotorPWMPin2, LOW);
  pinMode(MotorPWMPin2, OUTPUT);
  digitalWrite(MotorPWMPin2, LOW);
  digitalWrite(MotorDirPin1, LOW);
  pinMode(MotorDirPin1, OUTPUT);
  digitalWrite(MotorDirPin1, LOW);
  digitalWrite(MotorDirPin2, LOW);
  pinMode(MotorDirPin2, OUTPUT);
  digitalWrite(MotorDirPin2, LOW);
}

void setupSensors()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
}

void setup()
{
  #ifdef WIFI_ENABLED
    setupWifi();
    setupOta();
  #endif

  setupMotors();
  setupSensors();
  pixels.begin();
  // disable serial to enable led
  //Serial.begin(115200);
}

void setMotor1Speed(int speed)  {
  int restrictedSpeed = constrain(speed, MinSpeed, MaxSpeed);
  if(restrictedSpeed >= 0) {
    analogWrite(MotorPWMPin1, restrictedSpeed);
    digitalWrite(MotorDirPin1, LOW);
  }
  else {
    analogWrite(MotorPWMPin1, -restrictedSpeed);
    digitalWrite(MotorDirPin1, HIGH);
  }
}

void setMotor2Speed(int speed)  {
  int restrictedSpeed = constrain(speed, MinSpeed, MaxSpeed);
  if(restrictedSpeed >= 0) {
    analogWrite(MotorPWMPin2, restrictedSpeed);
    digitalWrite(MotorDirPin2, LOW);
  }
  else {
    analogWrite(MotorPWMPin2, -restrictedSpeed);
    digitalWrite(MotorDirPin2, HIGH);
  }
}

void setMotorsSpeed(int speed)  {
  setMotor1Speed(speed);
  setMotor2Speed(speed);
}

void setLedColor(uint32_t color) {
  pixels.setPixelColor(0, color);
  pixels.show();
}

void setSpeed(motor motor, int speed) {
  if(motor == LEFT_MOTOR) {
    setMotor1Speed(speed * SpeedAdjustLeft); 
  } else {
    setMotor2Speed(speed * SpeedAdjustRight); 
  }
}

void run(motor motor, directions dir, int speed) {
  switch (dir) {
    case FORWARD:
        setSpeed(motor, speed);
        break;
    case BACK:
        setSpeed(motor, -speed);
        break;
  }
}

void run(motor motor, directions dir) {
  run(motor, dir, MaxSpeed);
}

void stopMotors() {
  setSpeed(LEFT_MOTOR, SpeedStopped);
  setSpeed(RIGHT_MOTOR, SpeedStopped);
}

void readSensors() 
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  durationFront = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distanceFront = durationFront*0.0343/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance front: ");
  Serial.println(distanceFront);
  
  delay(1);
  
  // Clears the trigPin
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  durationSide = pulseIn(echoPin2, HIGH);
  // Calculating the distance
  distanceSide = durationSide*0.0343/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance side: ");
  Serial.println(distanceSide);
}

void turnRight() {
  setLedColor(green);
  // try to turn right
  run(LEFT_MOTOR, FORWARD); 
  setSpeed(RIGHT_MOTOR, SpeedStopped);
  delay(250);
}

void turnLeft() {
  setLedColor(green);
  // try to turn right
  run(RIGHT_MOTOR, FORWARD); 
  setSpeed(LEFT_MOTOR, SpeedStopped); 
  delay(250);
}

void goForward() {
  run(RIGHT_MOTOR, FORWARD); 
  run(LEFT_MOTOR, FORWARD);
  delay(250); 
}

void goBackward() {
  run(RIGHT_MOTOR, BACK); 
  run(LEFT_MOTOR, BACK);
  delay(250); 
}

void loop()
{
  #ifdef WIFI_ENABLED
    ArduinoOTA.handle();
    // Skip all other logic if we're running an OTA update
    if (OTAStarted) {
      return;
    }
    
    server.handleClient(); 
  #endif
  
  readSensors();
  setLedColor(blue);

  if(shouldGoForward) {
    goForward();
    shouldGoForward = false;
  }

  if(shouldGoBackward) {
    goBackward();
    shouldGoBackward = false;
  }

  if(shouldTurnLeft) {
    turnLeft();
    shouldTurnLeft = false;
  }

  if(shouldTurnRight) {
    turnRight();
    shouldTurnRight = false;
  }

  if(stopped) {
    stopMotors();
  } else {
    if(distanceFront > 10) {
      if(distanceFront > 100) {
        // run motors with max speed
        run(RIGHT_MOTOR, FORWARD); 
        run(LEFT_MOTOR, FORWARD);  
      } else {
        // run motors with half speed if obstacle is closed then 1m
        run(RIGHT_MOTOR, FORWARD, MaxSpeed / 2); 
        run(LEFT_MOTOR, FORWARD, MaxSpeed / 2);  
      }
    } 
    else {
      if(distanceSide > 10) {
        turnRight();
      } else {
        turnLeft();
      }
    } 
  }
  
  delay(100);
}