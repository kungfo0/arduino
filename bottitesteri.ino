#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

int moottori1PWMPin = D8;
int moottori1SuuntaPin = D7;
int moottori2PWMPin = D5;
int moottori2SuuntaPin = D6;

int sensori1TrigPin = D2;
int sensori1EchoPin = D1;
int sensori2TrigPin = D4;
int sensori2EchoPin = D3;

uint16_t pixelsLength = 1;
uint8_t pixelsPin = 3;
// neoPixelType pixelsType = NEO_RGBW + NEO_KHZ800;
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelsLength, pixelsPin, pixelsType);

long durationFront;
long durationSide;
int distanceFront;
int distanceSide;

#define colorSaturation 128
NeoPixelBus<NeoRgbFeature, Neo400KbpsMethod> pixels(pixelsLength, pixelsPin);

/*
uint32_t red = pixels.Color(255, 0, 0);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t blue = pixels.Color(0, 0, 255);
uint32_t white = pixels.Color(255, 255, 255, 255);
*/

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

#define WLAN_SSID "HUAWEI P20 lite"
#define WLAN_PASS "kerpele1"
#define HOSTNAME "botti-"

void setup_wifi() 
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
}

void setup() {
  //sensorit
  pinMode(sensori1TrigPin, OUTPUT); // Sets the sensori1TrigPin as an Output
  pinMode(sensori1EchoPin, INPUT); // Sets the echoPin as an Input

  pinMode(sensori2TrigPin, OUTPUT); // Sets the sensori1TrigPin as an Output
  pinMode(sensori2EchoPin, INPUT); // Sets the echoPin as an Input
  // moottorit
  digitalWrite(moottori1PWMPin, LOW);
  pinMode(moottori1PWMPin, OUTPUT);
  digitalWrite(moottori1PWMPin, LOW);
  digitalWrite(moottori2PWMPin, LOW);
  pinMode(moottori2PWMPin, OUTPUT);
  digitalWrite(moottori2PWMPin, LOW);
  digitalWrite(moottori1SuuntaPin, LOW);
  pinMode(moottori1SuuntaPin, OUTPUT);
  digitalWrite(moottori1SuuntaPin, LOW);
  digitalWrite(moottori2SuuntaPin, LOW);
  pinMode(moottori2SuuntaPin, OUTPUT);
  digitalWrite(moottori2SuuntaPin, LOW);
  
  Serial.begin(115200);
  while (!Serial); // wait for serial attach

  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();

  Serial.println();
  Serial.println("Running...");
}

void testLed(int ledNumber) {
  Serial.print("ledi: ");
  Serial.print(ledNumber);
  Serial.println(" punainen");
  pixels.SetPixelColor(ledNumber, red);
  pixels.Show();
  delay(1000);
  Serial.print("ledi: ");
  Serial.print(ledNumber);
  Serial.println(" vihreä");
  pixels.SetPixelColor(ledNumber, green);
  pixels.Show();
  delay(1000);
  Serial.print("ledi: ");
  Serial.print(ledNumber);
  Serial.println(" sininen");
  pixels.SetPixelColor(ledNumber, blue);
  pixels.Show();
  delay(1000);
  Serial.print("ledi: ");
  Serial.print(ledNumber);
  Serial.println(" valkoinen");
  pixels.SetPixelColor(ledNumber, white);
  pixels.Show();
  delay(1000);
  Serial.print("ledi: ");
  Serial.print(ledNumber);
  Serial.println(" musta");
  pixels.SetPixelColor(ledNumber, black);
  pixels.Show();
  delay(1000);
}

void asetaMoottorin1Nopeus(int nopeus) {
    int restrictedSpeed = constrain(nopeus, -1023, 1023);
    if (restrictedSpeed >= 0) {
        analogWrite(moottori1PWMPin, restrictedSpeed);
        digitalWrite(moottori1SuuntaPin, LOW);
    } else {
        analogWrite(moottori1PWMPin, -restrictedSpeed);
        digitalWrite(moottori1SuuntaPin, HIGH);
    }
}

void asetaMoottorin2Nopeus(int nopeus) {
    int restrictedSpeed = constrain(nopeus, -1023, 1023);
    if (restrictedSpeed >= 0) {
        analogWrite(moottori2PWMPin, restrictedSpeed);
        digitalWrite(moottori2SuuntaPin, LOW);
    } else {
        analogWrite(moottori2PWMPin, -restrictedSpeed);
        digitalWrite(moottori2SuuntaPin, HIGH);
    }
}

void testMotors() {
  Serial.println("Moottori 1 nollasta maksimiarvoon");
  for(int i=0;i<1023;i++) {
    asetaMoottorin1Nopeus(i);
    delay(5);
  }
  delay(250);
  Serial.println("Moottori 1 maksimiarvosta nollaan");
  for(int i=1023;i>=0;i--) {
    asetaMoottorin1Nopeus(i);
    delay(5);
  }

  Serial.println("Moottori 2 nollasta maksimiarvoon");
  for(int i=0;i<1023;i++) {
    asetaMoottorin2Nopeus(i);
    delay(5);
  }
  delay(250);
  Serial.println("Moottori 2 maksimiarvosta nollaan");
  for(int i=1023;i>=0;i--) {
    asetaMoottorin2Nopeus(i);
    delay(5);
  }

  Serial.println("Moottori 1 nollasta maksimiarvoon - taaksepäin");
  for(int i=0;i>-1023;i--) {
    asetaMoottorin1Nopeus(i);
    delay(5);
  }
  delay(250);
  Serial.println("Moottori 1 maksimiarvosta nollaan - taaksepäin");
  for(int i=-1023;i<=0;i++) {
    asetaMoottorin1Nopeus(i);
    delay(5);
  }

  Serial.println("Moottori 2 nollasta maksimiarvoon - taaksepäin");
  for(int i=0;i>-1023;i--) {
    asetaMoottorin2Nopeus(i);
    delay(5);
  }
  delay(250);
  Serial.println("Moottori 2 maksimiarvosta nollaan - taaksepäin");
  for(int i=-1023;i<=0;i++) {
    asetaMoottorin2Nopeus(i);
    delay(5);
  }
}

void testLeds() {
  Serial.println("Testaan ledejä..");
  // this resets all the neopixels to an off state
  pixels.Begin();
  pixels.Show();
  for(int i = 0; i<pixelsLength;i++) {
    testLed(0);
  }
}

void readSensors() {
    // Clears the sensori1TrigPin
    digitalWrite(sensori1TrigPin, LOW);
    delayMicroseconds(2);

    // Sets the sensori1TrigPin on HIGH state for 10 micro seconds
    digitalWrite(sensori1TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensori1TrigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    durationFront = pulseIn(sensori1EchoPin, HIGH);
    // Calculating the distance
    distanceFront = durationFront * 0.0343 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance front: ");
    Serial.println(distanceFront);

    delay(1);

    // Clears the sensori1TrigPin
    digitalWrite(sensori2TrigPin, LOW);
    delayMicroseconds(2);

    // Sets the sensori1TrigPin on HIGH state for 10 micro seconds
    digitalWrite(sensori2TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensori2TrigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    durationSide = pulseIn(sensori2EchoPin, HIGH);
    // Calculating the distance
    distanceSide = durationSide * 0.0343 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance side: ");
    Serial.println(distanceSide);
}

void testSensors() {
  Serial.println("Luetaan sensoreita");
  for(int i=0;i<1000;i++) {
    readSensors();
    delay(8);
  }
}

void loop() {
  int read;
  askTestMotors:
    Serial.println("Testataanko moottorit? k/e");
    while(!Serial.available()) { }
    read = Serial.read();
   // Serial.print("read: ");
   // Serial.println(read);
    if(read == 107 || read == 75) { // k
       testMotors();
    } else if(read == 101 || read == 69) { // e
      goto askSensors;
    } else{
      goto askTestMotors;
    }
  askSensors:
    Serial.println("Testataanko sensorit? k/e");
    while(!Serial.available()) { }
    read = Serial.read();
    if(read == 107 || read == 75) { // k
       testSensors();
    } else if(read == 101 || read == 69) { // e
      goto askLeds;
    } else{
      goto askSensors;
    }
  askLeds:
    Serial.println("Testataanko ledit? k/e");
    while(!Serial.available()) { }
    read = Serial.read();
    if(read == 107 || read == 75) { // k
       testLeds();
    } else if(read == 101 || read == 69) { // e
      goto askTestMotors;
    } else{
      goto askLeds;
    }
  delay(500);
}
