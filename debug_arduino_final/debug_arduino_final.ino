#include <Servo.h>                                            //import library
#define Number_servos 4                                       //number of servos used
Servo myservo[Number_servos];                                 //activating servos
int Servo_pins[Number_servos] = { 9, 10, 11, 12 };            //define the pin we are going to use for the servos
int servoL[Number_servos] = { 40, 0, 60, 65 };                //Servo limits of the movement (Left)
int servoR[Number_servos] = { 160, 180, 130, 145 }; //Servo limits of the movement (Right)
int val[Number_servos] = { 90, 90, 90, 90 } int tdelay = 10;  //Delay for each incremental (1°) movement of servo

void setup() {                               //happens only when u turn it on
  Serial.begin(115200);                      //start serial communication (must match the sender's baud rate)
  for (int i = 0; i < Number_servos; i++) {  //loop from 1 to 4 servos for the (list)
    myservo[i].attach(Servo_pins[i]);        //loop from 1 to 4 to connect servo to pin
  }
}

void printPositions() {  //serial information for debug
  for (int i = 0; i < Number_servos; i++) {
    Serial.print("S");
    Serial.print(i + 1);
    Serial.print(":");
    Serial.print(val[i]);
    Serial.print(",");
  }
  Serial.println();
}

void loop() {                                               // Loop that appends constantly
  for (int i = 0; i < Number_servos; i++) {                 //loop from 1 to 4 servos for the (list)
    for (int angle = 90; angle <= servoR[i]; angle += 1) {  //gradually (1°) move the each servo right
      myservo[i].write(angle);                              //upgrade servo to new position
      val[i] = angle;                                       //read the angle and add it to the specific value of the servo
      printPositions();                                     //print the serial of the action done
      delay(tdelay);                                        //delay
    };
    for (int angle = servoR[i]; angle >= servoL[i]; angle -= 1) {  // sweep back to other limmit
      myservo[i].write(angle);
      val[i] = angle;
      printPositions();
      delay(tdelay);
    }
    for (int angle = servoL[i]; angle <= 90; angle += 1) {  // go back to default position 90°
      myservo[i].write(angle);
      val[i] = angle;
      printPositions();
      delay(tdelay);
    }
  }
}
