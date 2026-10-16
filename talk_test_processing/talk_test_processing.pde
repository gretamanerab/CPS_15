import processing.serial.*;   // library for serial messaging

Serial port;
String lastSent = "nothing yet";
String lastReceived = "nothing yet";

void setup() {
  size(420, 160);
  println("Available serial ports:");
  printArray(Serial.list());

  // find the Arduino automatically: on a Mac its name contains "cu.usbmodem"
  String portName = null;
  for (String p : Serial.list()) {
    if (p.contains("/dev/cu.usbserial-0001")) {
      portName = p;
    }
  }
  if (portName == null) {
    println("Arduino not found, check the USB cable");
    exit();
    return;
  }

  println("Connecting to " + portName);
  port = new Serial(this, portName, 9600);  // same speed as the Arduino
  port.bufferUntil('\n');                   // read the Arduino one full line at a time
  delay(2000);                              // give the Arduino time in case it restarts
  sendText("HELLO");                        // first message: check the connection
}

void draw() {
  background(30);
  fill(255);
  textSize(14);
  text("Press 1, 2 or 3 to send a number, H to send HELLO", 20, 35);
  text("Sent:      " + lastSent, 20, 80);
  text("Received:  " + lastReceived, 20, 115);
}

// send a line of text to the Arduino
void sendText(String msg) {
  port.write(msg + "\n");          // \n marks the end of the message
  lastSent = msg;
  println("Processing sent: " + msg);
}

// runs when a key is pressed
void keyPressed() {
  if (key == '1' || key == '2' || key == '3') {
    sendText(str(key));            // send the number as text
  }
  if (key == 'h' || key == 'H') {
    sendText("HELLO");
  }
}

// runs by itself when the Arduino sends a full line
void serialEvent(Serial p) {
  String reply = p.readStringUntil('\n');
  if (reply != null) {
    lastReceived = trim(reply);
    println("Arduino replied: " + lastReceived);
  }
}
