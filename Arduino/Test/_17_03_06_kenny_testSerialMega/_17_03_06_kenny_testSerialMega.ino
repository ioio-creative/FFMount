String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
long lastBlink;
void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial1.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(13, OUTPUT);
  pinMode( 19, INPUT_PULLUP );
  
}

void loop() {
  serialEvent1();
//  digitalWrite(13, HIGH);
//  delay(500);
  //(millis() < lastBlink + 100 ) ? digitalWrite(13, HIGH) : digitalWrite(13, LOW);
  // print the string when a newline arrives:
//  if (stringComplete) {
//    Serial.println(inputString);
//    // clear the string:
//    inputString = "";
//    stringComplete = false;
//  }
  
//  digitalWrite(13, LOW);
//  delay(500);
  (millis() < lastBlink + 100 ) ? digitalWrite(13, HIGH) : digitalWrite(13, LOW);
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial1.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent1() {

//  while (Serial.available()) {
//    // get the new byte:
//    lastBlink = millis();
//    char inChar = (char)Serial.read();
//    // add it to the inputString:
//    inputString += inChar;
//    // if the incoming character is a newline, set a flag
//    // so the main loop can do something about it:
//    if (inChar == '\n') {
//      stringComplete = true;
//    }
//  }

  while (Serial1.available()) {
    // get the new byte:
    lastBlink = millis();
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
