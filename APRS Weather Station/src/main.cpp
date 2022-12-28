#include <Arduino.h>
#include <AD9850SPI.h>
#include <SPI.h>
#include <string.h>

const int W_CLK_PIN = 13;
const int FQ_UD_PIN = 4;
const int RESET_PIN = 5;

double freq = 1000000;
double trimFreq = 124999500;

int phase = 0;

const byte maxChars = 32;
char receivedChars[maxChars];   // an array to store the received data

void printParams(void);
bool recCommand(void);

void setup(){
  Serial.begin(9600);
  DDS.begin(W_CLK_PIN, FQ_UD_PIN, RESET_PIN);
  DDS.calibrate(trimFreq);
  DDS.setfreq(freq, phase);
  Serial.println("+-------------------------+");
  Serial.println("|   AD9850 Test Program   |");
  Serial.println("| Joseph R. Freeston 2022 |");
  Serial.println("+-------------------------+");
  Serial.println();
  printParams();
  Serial.println();
  Serial.println();
  Serial.println("Enter a command.");
  Serial.println("Begin with either R, F, U, P, or D and an optional float following the first character.");
  Serial.print(">>> ");

}

void loop(){
  if(recCommand()) {
    if(receivedChars[0] == 'F') {         // Set frequency
      freq = atof(&receivedChars[1]);
      DDS.setfreq(freq, phase);
    } else if(receivedChars[0] == 'R') {  // Set reference
      trimFreq = atof(&receivedChars[1]);
      DDS.calibrate(trimFreq);
    } else if(receivedChars[0] == 'P') {  // Set phase
      phase = atof(&receivedChars[1]);
      DDS.setfreq(freq, phase);
    } else if(receivedChars[0] == 'U') {  // Up
      DDS.up();
    } else if(receivedChars[0] == 'D') {  // Down
      DDS.down();
    } else {
      Serial.println("Invalid command. Begin with either R, F, U, P, or D and an optional float following the first character.");
    }
    printParams();
    Serial.print("\n>>> ");
  }
}

void printParams() {
  Serial.println("------------------------------");
  Serial.print("Reference set to ");
  Serial.print(trimFreq, DEC);
  Serial.println("Hz");
  Serial.print("Frequency set to ");
  Serial.print(freq, DEC);
  Serial.println("Hz");
  Serial.print("Phase set to ");
  Serial.print(phase*11.25, DEC);
  Serial.println("°");
  Serial.println("------------------------------");
}

bool recCommand() {
  static byte i = 0;
  const char endMarker = '\n';
  char rc;

  while (Serial.available() > 0) {
    rc = Serial.read();
    Serial.write(rc);

    if(rc != endMarker) {
      receivedChars[i] = rc;
      if(i < maxChars) i++;
    } else {
      receivedChars[i] = '\0';
      i = 0;
      return true;
    }
  }
  return false;
}