#include <TimerOne.h>

// Define encoder pins
const uint8_t REncoderPin = 2;
const uint8_t LEncoderPin = 3;

// Define Encoder Counters
volatile uint32_t LCoderCount = 0;
volatile uint32_t RCoderCount = 0;

// Define Sample Time and Freqency of Serial Print Interrupt
const uint32_t freq = 50; //Hz
const uint32_t Ts = 1000000/freq;  //Sample Time in microseconds

//Supporting Functions
void incrementLEncoderCount()
{
// Description: Increase the left encoder counter by 1
  LCoderCount++;
}

void incrementREncoderCount()
{
  //Description: Increase the right encoder counter by 1
  RCoderCount++;
}



void printEncoderCount()
{
  char buff1[4];
  char buff2[4];
  sprintf(buff1, "%#4u", LCoderCount);
  Serial.print("Left Encoder: ");
  Serial.print(buff1);
  Serial.print("        Right Encoder: ");
  sprintf(buff2, "%#4u", RCoderCount);
  Serial.println(buff2);
}


void setup() {
  //Initialize Serial
  Serial.begin(9600); 

  // Interrupts to increment encoder counts
  attachInterrupt(digitalPinToInterrupt(LEncoderPin), incrementLEncoderCount, CHANGE);
  attachInterrupt(digitalPinToInterrupt(REncoderPin), incrementREncoderCount, CHANGE);

  // Initialize Timer for Serial Printing
  Timer1.initialize(Ts);
  Timer1.attachInterrupt(printEncoderCount);

}

void loop() {
  // put your main code here, to run repeatedly:

}
