#include <Servo.h>       //servo library
#define Number_servos 4  //number of servos used

Servo myservo[Number_servos];                       //activating servos
int Servo_pins[Number_servos] = { 9, 10, 11, 12 };  //define the pin we are going to use for the servos
int classCode = -1;                                 //speak w wekinator
int servP[Number_servos] = { 0, 0, 0, 0 };          //Variable for setting desired position of servos
int servoNow[Number_servos] = { 90, 90, 90, 90 };   //Start Defaul position (variable for saving current servo position)
int minP[Number_servos] = { 0, 45, 0, 65 };         //(case 3) limits of the movement
int maxP[Number_servos] = { 0, 135, 0, 145 };       //(case 3) limits of the movement
int dir[Number_servos] = { 1, 1, 1, 1 };            // (case 3) Swap direction of the movement
unsigned long previousMillis = 0;                   // variable to store last time arduino was updated
const long interval = 10;                           //delay wanted

void setup() {                               //happens only when u turn it on
  Serial.begin(9600);                        //start serial communication (must match the sender's baud rate)
  for (int i = 0; i < Number_servos; i++) {  //loop from 1 to 4 servos for the (list)
    myservo[i].attach(Servo_pins[i]);        //loop from 1 to 4 to connect servo to pin
    myservo[i].write(90);                    //Move every servo to default position at the start
  }
}

void loop() {                              // Loop that appends constantly
  unsigned long currentMillis = millis();  // Start storing last time it moved
  if (Serial.available()) {                //reads for info from processing
    classCode = Serial.read();             //make info from processing become classcode
  }

  switch (classCode) {  // switch for cases

    case 1:
      {                  // Open hand
        servP[0] = 85;   //position of 0 servo on pin 9
        servP[1] = 90;   //position of 1 servo on pin 10
        servP[2] = 130;  //position of 2 servo on pin 11
        servP[3] = 90;   //position of 3 servo on pin 12
        break;           //end of case
      }

    case 2:
      {  // Close hand
        servP[0] = 45;
        servP[1] = 90;
        servP[2] = 70;
        servP[3] = 90;
        break;
      }

    case 3:
      {  // Arm down, then rotate + mouth
        servP[0] = 120;
        servP[1] = 90;
        servP[2] = 70;
        servP[3] = 85;
        break;
      }

    default:
      {  // Everything back to 90°
        servP[0] = 90;
        servP[1] = 90;
        servP[2] = 90;
        servP[3] = 90;
        break;
      }
  }

  if (currentMillis - previousMillis >= interval) {  //check if the interval is achived
    previousMillis = currentMillis;                  // updates arduino clock

    // so the servos move one step every 10 ms (smooth)
    if (classCode <= 2) {                        // what the servo should do with the new position in case 1 and 2
      for (int i = 0; i < Number_servos; i++) {  // from 1 to 4
        if (servoNow[i] < servP[i]) {            //checking if position servo is LOWER then the new position we want the servo motor to go
          servoNow[i] = servoNow[i] + 1;         // gradually moves the servo to the new position
        } else if (servoNow[i] > servP[i]) {     // checking if position servo is HIGHER then the new position we want the servo motor to go
          servoNow[i] = servoNow[i] - 1;         // gradually moves the servo to the new position
        }
        myservo[i].write(servoNow[i]);  // upgrade the now postion of the servo
      }
    } else if (classCode == 3) {                                                //what the servo should do for the case 3
      for (int i = 0; i < Number_servos; i++) {                                 // from 1 to 4
        bool armDown = (servoNow[0] == servP[0]) && (servoNow[2] == servP[2]);  // check if servo 0 and 2 arrived to the right position to start animation
        if (i == 0 || i == 2) {                                                 // if 0 and 2 are not in position gradually move them to the positon
          if (servoNow[i] < servP[i]) {         //checking if position servo is LOWER then the new position we want the servo motor to go
            servoNow[i] = servoNow[i] + 1;      // gradually moves the servo to the new position
          } else if (servoNow[i] > servP[i]) {  // checking if position servo is HIGHER then the new position we want the servo motor to go
            servoNow[i] = servoNow[i] - 1;      // gradually moves the servo to the new position
          }
          myservo[i].write(servoNow[i]);  // upgrade the now postion of the servo
        } else if (armDown) {             // if armDown now is true and (0 and 2 in position) start animation Motors 2 and 4 swing
          if (servoNow[i] <= minP[i]) {
            dir[i] = 1;    // go up
          }
          if (servoNow[i] >= maxP[i]) {
            dir[i] = -1;   // go down
          }

          servoNow[i] = servoNow[i] + dir[i];   // move one step in the chosen direction prongs open and close smoothly
          myservo[i].write(servoNow[i]);
        }
      }
    }
  }
}   