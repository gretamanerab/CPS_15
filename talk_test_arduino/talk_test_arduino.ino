/*
   talk_test (Arduino)
   Answers every message from Processing with a line of text.
   Uses no pins, so nothing connected to the board moves.
*/

void setup() {
  Serial.begin(9600);          // same speed as Processing
  Serial.setTimeout(50);       // don't wait more than 50 ms for the end of a message
}

void loop() {
  if (Serial.available()) {                      // something arrived from Processing
    String msg = Serial.readStringUntil('\n');   // read one full line of text
    msg.trim();                                  // remove spaces and the new line

    if (msg == "HELLO") {                        // Processing is checking the connection
      Serial.println("READY");                   // answer
    } else {
      Serial.print("Arduino got: ");             // repeat back whatever arrived
      Serial.println(msg);
    }
  }
}