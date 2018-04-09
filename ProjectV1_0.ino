#include <DueTimer.h>

// Define IR Sensor pins
const int IR0 = A0;
const int IR1 = A1;
const int IR2 = A2;
const int IR3 = A3;
const int IR4 = A4;
const int IR5 = A5;
const int IR6 = A6;
const int IR7 = A7;
const int IR8 = A8;
const int IR9 = A9;

volatile int irVals[10];

// Define Motor Pins
const int in1 = 11; // right motor
const int in2 = 10; // right motor
const int in3 = 9; // left motor
const int in4 = 8; // left motor
const int enA = 3; // right motor
const int enB = 2; // left motor

// Define sampling frequnecy
const int freq = 50; //Hz

// Define IR Sensor Weights
int weights[10] = {-1, -1, -1, -1, -1, 1, 1, 1, 1, 1};

// Motor voltages
const int fullVolt = 100;
const int floorVolt = 0;
int voltRange = fullVolt - floorVolt;
int maxOffset;
int scaling;


void InSerRout(){
  int tot = 0;
  for (int ind = 0; ind < 10; ind++){
    tot += weights[ind]*irVals[ind];
//    Serial.print(String(irVals[ind]) + "   ");
  }

  int leftMotorVolt = min(fullVolt, fullVolt - tot*scaling);
  int rightMotorVolt = min(fullVolt, fullVolt + tot*scaling);

  analogWrite(enA, rightMotorVolt);
  analogWrite(enB, leftMotorVolt);

//  Serial.print(String(leftMotorVolt) + "   ");
//  Serial.print(String(rightMotorVolt));
//  Serial.print("\n");
   
}




void setup() {

//  // Begin serial
//  Serial.begin(115200);

  // Calc maxoffset
  for (int i = 0; i < 10; i++){
    if (weights[i] > 0){
      maxOffset += weights[i]; 
    }
    scaling = voltRange/maxOffset;
  }

  
  // Set IR Pins
  pinMode(IR0, INPUT);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(IR6, INPUT);
  pinMode(IR7, INPUT);
  pinMode(IR8, INPUT);
  pinMode(IR9, INPUT);

  // Set Motor Direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH); 

  // Set Timer Interrupt
 Timer3.attachInterrupt(InSerRout);
 Timer3.setFrequency(freq);
 Timer3.start();
  

}

void loop() {
  // Sample IR Sensors
  irVals[0] = (analogRead(IR0) < 500);
  irVals[1] = (analogRead(IR1) < 500);
  irVals[2] = (analogRead(IR2) < 500);
  irVals[3] = (analogRead(IR3) < 500);
  irVals[4] = (analogRead(IR4) < 500);
  irVals[5] = (analogRead(IR5) < 500);
  irVals[6] = (analogRead(IR6) < 500);
  irVals[7] = (analogRead(IR7) < 500);
  irVals[8] = (analogRead(IR8) < 500);
  irVals[9] = (analogRead(IR9) < 500);

}
