/*
 * Coded by 
 * Eliemer Velez: eliemer.velez@upr.edu
 * Jaime Ginorio: jaime.ginorio@upr.edu
 */
#include <PS4BT.h>
#include <usbhub.h>
#include <Servo.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb);
BTD Btd(&Usb);
PS4BT PS4(&Btd);

int leftSpeed, rightSpeed;

double leftSense = 0.45;
double rightSense = 0.45;

Servo DC; // DC MOTOR

int leftPin = 5, rightPin = 6;
int leftIn1Pin = 7, leftIn2Pin = 2;
int rightIn1Pin = 4, rightIn2Pin = 3;

int angle = 45;

boolean leftDir, rightDir, gate;

int joyLeft, joyRight;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1);
  }
  DC.attach(22); //
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  pinMode(leftIn1Pin, OUTPUT);
  pinMode(leftIn2Pin, OUTPUT);
  pinMode(rightIn1Pin, OUTPUT);
  pinMode(rightIn2Pin, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  Usb.Task();

  if (PS4.connected()) {
    if (PS4.getAnalogHat(LeftHatY) > 147 || PS4.getAnalogHat(LeftHatY) < 107 || PS4.getAnalogHat(RightHatY) > 147 || PS4.getAnalogHat(RightHatY) < 107) {

      joyLeft = PS4.getAnalogHat(LeftHatY);
      joyRight = PS4.getAnalogHat(RightHatY);

      if (joyLeft < 127) {   //Going Forward
        leftDir = true;
        leftSpeed = map(joyLeft, 127, 0, 0, 255 * leftSense);
      } else {
        leftDir = false;
        leftSpeed = map(joyLeft, 128, 255, 0, 255 * leftSense);
      }

      if (joyRight < 127) {
        rightDir = true;
        rightSpeed = map(joyRight, 127, 0, 0, 255 * rightSense);
      } else {
        rightDir = false;
        rightSpeed = map(joyRight, 128, 255, 0, 255 * rightSense);
      }

      //Left Motor PWM
      analogWrite(leftPin, leftSpeed);
      digitalWrite(leftIn1Pin, ! leftDir);
      digitalWrite(leftIn2Pin, leftDir);

      //Right Motor PWM
      analogWrite(rightPin, rightSpeed);
      digitalWrite(rightIn1Pin, rightDir);
      digitalWrite(rightIn2Pin, ! rightDir);

    } else {

      //Left Motor Float
      analogWrite(leftPin, 0);
      digitalWrite(leftIn1Pin, HIGH);
      digitalWrite(leftIn2Pin, HIGH);

      //Right Motor Float
      analogWrite(rightPin, 0);
      digitalWrite(rightIn1Pin, HIGH);
      digitalWrite(rightIn2Pin, HIGH);

    }

    if (PS4.getButtonPress(L1) || PS4.getButtonPress(R1)) {
      if (PS4.getButtonPress(L1)) {

        //Left Motor Brake
        analogWrite(leftPin, 0);
        digitalWrite(leftIn1Pin, LOW);
        digitalWrite(leftIn2Pin, LOW);

      }
      if (PS4.getButtonPress(R1)) {

        //Right Motor Brake
        analogWrite(rightPin, 0);
        digitalWrite(rightIn1Pin, LOW);
        digitalWrite(rightIn2Pin, LOW);

      }
    }

    if (PS4.getButtonClick(UP)) {

      leftSense = min(1, leftSense + 0.05);
      rightSense = min(1, rightSense + 0.05);

    }

    if (PS4.getButtonClick(DOWN)) {

      leftSense = max(0, leftSense - 0.05);
      rightSense = max(0, rightSense - 0.05);

    }

    if (PS4.getButtonClick(LEFT)) {
      leftSense = max(0, leftSense - 0.01);
    }

    if (PS4.getButtonClick(RIGHT)) {
      leftSense = min(1, leftSense + 0.01);
    }

    if (PS4.getButtonClick(SQUARE)) {
      rightSense = max(0, rightSense - 0.01);
    }

    if (PS4.getButtonClick(CIRCLE)) {
      rightSense = min(1, rightSense + 0.01);
    }

    gate = PS4.getAnalogButton(R2);

    Serial.print(F("\r\nlever: "));
    Serial.print(gate);

  }

  if ( ! gate) {
    DC.write(45);
  } else {
    DC.write(135);
  }
}

