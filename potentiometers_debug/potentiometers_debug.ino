// potentiometers 4 servos

#include <Servo.h>   //import library
#define Number_servos 4     //number of servos used
Servo myservo[Number_servos]; //activating servos
int Servo_pins[Number_servos] = { 10, 9, 11, 13 }; //define the pin we are going to use for the servos (servo 0, 1, 2, 3)
int Pot_pins[Number_servos] = { A0, A1, A2, A3 }; //define the pin we are going to use for the potentiometers (pot 0, 1, 2, 3)
int servoL[Number_servos] = { 0, 40, 60, 65 }; //Servo limits of the movement (Left)
int servoR[Number_servos] = { 180, 160, 130, 145 }; //Servo limits of the movement (Right)
int val[Number_servos] = { 90, 90, 90, 90 }; //angle of each servo
int tdelay = 10; //Delay between each reading of the potentiometers

void setup() {    //happens only when u turn it on
  Serial.begin(115200); //start serial communication (must match the Serial Monitor speed)
  for (int i = 0; i < Number_servos; i++) {  //loop from 1 to 4 servos for the (list)
    myservo[i].attach(Servo_pins[i]);  //loop from 1 to 4 to connect servo to pin
    myservo[i].write(val[i]);  //start every servo at 90
  }
}

void printPositions() {  //serial information for debug
  for (int i = 0; i < Number_servos; i++) {  //loop from 1 to 4 servos for the (list)
    Serial.print("S");  //print S for servo
    Serial.print(i + 1);   //print the number of the servo
    Serial.print(":");
    Serial.print(val[i]);  //print the angle of the servo
    Serial.print(",");
  }
  Serial.println();  //go to a new line after the 4 servos
}

void loop() {  // Loop that appends constantly
  for (int i = 0; i < Number_servos; i++) {    //loop from 1 to 4 servos for the (list)
    int potValue = analogRead(Pot_pins[i]);  //read the potentiometer (0 to 1023)
    val[i] = map(potValue, 0, 1023, servoL[i], servoR[i]);     //turn the reading into an angle between the left and right limits
    myservo[i].write(val[i]);   //upgrade servo to new position
  }
  printPositions();   //print the serial of the action done
  delay(tdelay);   //delay
}
