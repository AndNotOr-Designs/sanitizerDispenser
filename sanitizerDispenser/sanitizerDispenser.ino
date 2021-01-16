// Board: "ArduinoNano"
// Processor: "ATmega328P (Old Bootloader)"
// Programmer: "Arduino as ISP"

// version information - reference in README.md
const float sanitizerDispenserVersion = 1.0;       
const String versionDate = "01/15/2021";                  

// debug info
const boolean debugOn = true;                          // debug to monitor - ****adds 2 second delay in sensor readings
const boolean superDebugOn = false;                     // advanced debugging with variable info and timers

#include <NewPing.h>                                    // ultrasonic sensor

// pin definitions
  // LEDs
  const int greenLedReady = 4;
  const int yellowLedRead = 3;
  const int redLedDispense = 2;
  // sensor
  const int TRIGGER_PIN = 8;                            // library uses full caps for these variables
  const int ECHO_PIN = 7;
  #define MAX_DISTANCE 200                              // Max distance to ping (in centimeters)
  // pump
  const int pump = 5;

  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);   // NewPing configuration

//variables
  int distance;                                         // for tracking current reading
  int previousDistance;                                 // reference
  int waitingForHand = 0;
  int prepPump = 0;
  int pumpSanitizer = 0;
  int count = 0;
  
void setup() {
  pinMode (greenLedReady, OUTPUT);
  pinMode (yellowLedRead, OUTPUT);
  pinMode (redLedDispense, OUTPUT);
  pinMode (pump, OUTPUT);

// serial monitor header
  Serial.begin(115200);                                 // monitor
  Serial.println();
  delay (1);
  Serial.println("/- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\\");
  Serial.print("|                                sanitizerDispenser v");
  Serial.print(sanitizerDispenserVersion);
  Serial.println("                                   |");
  Serial.print("|                                     --");
  Serial.print(versionDate);
  Serial.println("--                                        |");
  Serial.println("|                                         And!Or                                            |");
  Serial.println("|                                                                                           |");
  Serial.print("|                       Debug Status: ");
  if (debugOn == true) {
    Serial.print("on, Super Debug Status: ");
    if (superDebugOn == true) {
      Serial.println("on                            |");
    }
    else {
      Serial.println("off                           |");
    }
  }
  else {
    Serial.print("off, Super Debug Status: ");
    if (superDebugOn == true) {
      Serial.println("on                           |");
    }
    else {
      Serial.println("off                          |");
    }
  }
  Serial.println("\\- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -/");
// end serial monitor header


}

void outputControl() {
  if (prepPump == 1) {
    digitalWrite (greenLedReady, LOW);
    digitalWrite (yellowLedRead, HIGH);
    digitalWrite (redLedDispense, LOW);
    digitalWrite (pump, LOW);
    prepPump = 0;
  } else if (waitingForHand == 1) {
    digitalWrite (greenLedReady, HIGH);
    digitalWrite (yellowLedRead, LOW);
    digitalWrite (redLedDispense, LOW);
    digitalWrite (pump, LOW);
    waitingForHand = 0;
  } else if (pumpSanitizer == 1) {
    digitalWrite (greenLedReady, LOW);
    digitalWrite (yellowLedRead, LOW);
    digitalWrite (redLedDispense, HIGH);
    delay(5);
    digitalWrite (pump, HIGH);
    delay(5);
    digitalWrite (pump, LOW);
    pumpSanitizer = 0;
  }
}

void sanitizerPing() {
  Serial.println("ping");
  distance = sonar.ping_cm();
  count = 1;
  if (distance != previousDistance) {
    previousDistance = distance;
    count = 0;
  }
  if ((distance <= 10) && (count = 0)) {
    waitingForHand = 0;
    prepPump = 1;
    pumpSanitizer = 0;
  } else if ((distance >= 11) && (count = 0)) {
    waitingForHand = 1;
    prepPump = 0;
    pumpSanitizer = 0;
  } else if (count = 1) {
    waitingForHand = 0;
    prepPump = 0;
    pumpSanitizer = 1;
    
  }
}

void debugPrint() {
  if (debugOn == true) {
    Serial.print("Ping: ");
    Serial.print(sonar.ping_cm());
    Serial.println(" cm");
    Serial.print("waitingForHand: ");
    Serial.println(waitingForHand);
    Serial.print("prepPump: ");
    Serial.println(prepPump);
    Serial.print("pumpSanitizer: ");
    Serial.println(pumpSanitizer);
    Serial.print("count: ");
    Serial.println(count);
    if (superDebugOn == true) {
      Serial.println();
    }
    delay(2000);                                        // delay in the print
  }
}

void loop() {
  // put your main code here, to run repeatedly:
    sanitizerPing();
    outputControl();
    delay(50);
    debugPrint();

}
