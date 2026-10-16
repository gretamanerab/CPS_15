import oscP5.*;             //import library for osc messaging
import netP5.*;             //import library for osc messaging
import processing.serial.*; //import library for serial messagin

OscP5 oscP5;                  //create variable
NetAddress myRemoteLocation;  //create variable
Serial port;                  //create variable
int arduinoMessage = -1;      //set connection to arudino to -1 to be sure no cases are activeted on arduino with no connection

void setup() {
  size(256, 150);
  println("Available serial ports:");               //print all the port available
  printArray(Serial.list());                        //
  port = new Serial(this, Serial.list()[0], 9600);  //connect to the selected port from the list and decided communication speed (same as arduino)
  oscP5 = new OscP5(this, 12000);                   //start osc comunication with wekinator at speacific port
}

void draw() {
  // draw a gradient from black to white, just to fill the window
  for (int i = 0; i < 256; i++) {
    stroke(i);
    line(i, 0, i, 150);
  }
  //int mouseMessage = (int)map(mouseX, 0, 256, 0, 4);    // turn on to check if the problem is osc messaging (for debugging)
  //updateArduino(mouseMessage);                          // turn on to check if the problem is osc messaging (for debugging)
}


void oscEvent(OscMessage theOscMessage) {                          //check form osc message
  if (theOscMessage.checkAddrPattern("/wek/outputs") == true) {    //checking if typetag is the right one ("/wek/outputs")
    if (theOscMessage.checkTypetag("f")) {                         //check for the amount of outputs
      float firstValue = theOscMessage.get(0).floatValue();        //extract osc values and add them in variable first value
      print("### received an osc message /test with typetag f.");  // printing osc value recived
      println(" values: " + firstValue);                           // printing osc value recived

      updateArduino((int)firstValue);  //converting value from float to int for arduino
      return;
    }
  }
  println("### received an osc message. with address pattern " + theOscMessage.addrPattern());  // if typetag is wrong it sends you the typetag the osc is sending
}

void updateArduino(int newMessage) {   //send message to arduino
  if (newMessage != arduinoMessage) {  //send message only if different from previus
    arduinoMessage = newMessage;       // if diferent upgrde message
    println(arduinoMessage);           //printing the message is sending
    port.write(arduinoMessage);        // sending to arduino
  }
}
