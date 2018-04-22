#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <math.h>

ESP8266WebServer server;
Ticker timer;

// Network Info
char* ssid = "BetterLateThanNever";
char* password = "PleaseWork";

//Blink pin
uint8_t pin_led = 16;

//Reference Angle
double angle;

//Define Sample Rate
const float freq = 7; //Hz
const float Ts = 1/freq; //Sample time in seconds

// Define Motor Pins
const uint8_t in1 = D1; // right motor
const uint8_t in2 = D2; // right motor
const uint8_t in3 = D3; // left motor
const uint8_t in4 = D4; // left motor
const uint8_t enR = D5; // right motor
const uint8_t enL = D6; // left motor

//Define motor voltage variables
long leftFullVolt = 1023;
long rightFullVolt = 1023;
int floorVolt = 224;


volatile int leftMotorVolt;
volatile int rightMotorVolt;


// PI Controller Variables
double ov_old = 0;
double ov_new = 0;
double e_old_old = 0;
double e_old = 0;
double e_new = 0;
double kp = 5;
double ki = 0;
double kd = 0;

//Toggle the LED
void toggleLED(){
  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send(204,""); 
}

void startRobot(){
  server.send(204,"");
  ov_new = 0;
  ov_old = 0;
  e_new = 0;
  e_old = 0;
  e_old_old = 0;
  delayMicroseconds(200);
  timer.attach(Ts, isrFunction); 
}

void stopRobot(){
  analogWrite(enR, 0);
  analogWrite(enL, 0);
  if (timer.active()){
    timer.detach();  
  }
  server.send(204,""); 
}


// Parse url for angle
void angleFromAdd(){
  String newAngle = server.arg("angle");
  String newLFV = server.arg("lfv");
  String newRFV = server.arg("rfv");
  String newKp = server.arg("kp");
  String newKi = server.arg("ki");
  String newKd = server.arg("kd");
  
  angle = newAngle.toFloat();
  if (newLFV!="default"){
    leftFullVolt = newLFV.toInt();
  }
  if (newRFV!="default"){
    rightFullVolt = newRFV.toInt();
  }
  if (newKp!="default"){
    kp = newKp.toFloat();
  }
  if (newKi!="default"){
    ki = newKi.toFloat();
  }
  if (newKd != "default"){
    kd = newKd.toFloat();
  }

  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send(204, "");
}


void isrFunction(){
  
  if (ki==0.0 && kd == 0.0){             // Check for ki and kd.  If both ki and kd = 0, then code for P controller
    e_new = -angle;
    ov_new = kp*e_new;
  }
  else{                     // If ki or kd are nonzero. Code PID controller
    e_old = e_new;
    e_new = -angle;
    ov_old = ov_new;
    ov_new = ov_new + (kp+ki+kd)*e_new - (kp + 2*kd)*e_old + kd*e_old_old;    
  }
  

  leftMotorVolt = max(min(leftFullVolt, leftFullVolt + round(ov_new)), -leftFullVolt);
  rightMotorVolt = max(min(rightFullVolt, rightFullVolt - round(ov_new)), -rightFullVolt);

  // Change direction of left motor
  if (leftMotorVolt < 0){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    leftMotorVolt = -leftMotorVolt;
  }
  else{
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

  // Change direction of right motor
  if (rightMotorVolt < 0){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rightMotorVolt = -rightMotorVolt;
  }
  else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  leftMotorVolt = map(leftMotorVolt, 0, leftFullVolt, floorVolt, leftFullVolt);
  rightMotorVolt = map(rightMotorVolt, 0, rightFullVolt, floorVolt, rightFullVolt);
  analogWrite(enR, rightMotorVolt);
  analogWrite(enL, leftMotorVolt);
}



void setup(){

  // Setup LED PIN
  pinMode(pin_led, OUTPUT);


  // Wifi Setup
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  server.on("/",[](){server.send(200,"text/plain","Hello world!");});
  server.on("/startYourEngines", startRobot);
  server.on("/stopYourEngines", stopRobot);
  server.on("/angleFromAddress", angleFromAdd);
  server.begin();


  // Initial Motor Pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enL, OUTPUT);
  pinMode(enR, OUTPUT);

  

  // Set Motor Direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  //

}

void loop(){
  server.handleClient();

}
