// Board: "ArduinoNano"
// Processor: "ATmega328P (Old Bootloader)"
// Programmer: "Arduino as ISP"

// version information - reference in README.md
const float sanitizerDispenserVersion = 1.0;       
const String versionDate = "01/16/2021";                  

// debug info
const boolean debugOn = false;                          // debug to monitor - ****adds 1 second delay in sensor readings
const boolean superDebugOn = false;                     // advanced debugging with variable info and timers

#include <NewPing.h>                                    // ultrasonic sensor

// pin definitions
  // LEDs
  const int greenLedReady = 2;
  const int yellowLedRead = 3;
  const int redLedDispense = 4;
  // sensor
  const int TRIGGER_PIN = 8;                            // library uses full caps for these variables
  const int ECHO_PIN = 7;
  #define MAX_DISTANCE 200                              // Max distance to ping (in centimeters)
  // pump
  const int pump = 5;

  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);   // NewPing configuration

//variables
  int takeReadings = 1;                                 // process the readings
  int distance;                                         // for tracking current reading
  int tooManyPumpsDistance;
  int previousDistance;                                 // reference
  int waitingForHand = 0;
  int prepPump = 0;
  int pumpSanitizer = 0;
  int initialHandSense = 0;
  int pumpCount = 0;
  int pausePump = 0;
  float resetTime = 0;

// timing
  float currentMillis;
  float timeStamp;
  float holdTime = 250;
  float pumpTime = 150;
  
  
void setup() {
  pinMode (greenLedReady, OUTPUT);
  pinMode (yellowLedRead, OUTPUT);
  pinMode (redLedDispense, OUTPUT);
  pinMode (pump, OUTPUT);

// Serial monitor header
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
// end Serial monitor header
  
  takeReadings = 1;
  pausePump = 0;
  if (debugOn == true) {                                // setting timer reference for tooManyPumps timing
    resetTime = 30000;                                  // 30 seconds
  } else {
    resetTime = 300000;                                 // 5 minutes
  }
}

void outputControl() {
  if (waitingForHand == 1) {
    digitalWrite (greenLedReady, HIGH);
    digitalWrite (yellowLedRead, LOW);
    digitalWrite (redLedDispense, LOW);
    digitalWrite (pump, LOW);
  } else if (prepPump == 1) {
    digitalWrite (greenLedReady, LOW);
    digitalWrite (yellowLedRead, HIGH);
    digitalWrite (redLedDispense, LOW);
    digitalWrite (pump, LOW);
  } else if (pumpSanitizer == 1) {
    digitalWrite (greenLedReady, LOW);
    digitalWrite (yellowLedRead, LOW);
    digitalWrite (redLedDispense, HIGH);
    delay(50);
    digitalWrite (pump, HIGH);
    delay(pumpTime);
    digitalWrite (pump, LOW);
    pumpCount ++;
    digitalWrite (greenLedReady, LOW);
    digitalWrite (yellowLedRead, HIGH);
    digitalWrite (redLedDispense, LOW);
    delay(1000);                                        // initiated pump, delay for hand to move away
  }
}
void flashingLights() {
  digitalWrite (greenLedReady, HIGH);
  digitalWrite (yellowLedRead, HIGH);
  digitalWrite (redLedDispense, HIGH);
  delay(100);
  digitalWrite (greenLedReady, LOW);
  digitalWrite (yellowLedRead, LOW);
  digitalWrite (redLedDispense, LOW);
  delay(100);
}

void tooManyPumps() {
  if (pumpCount >= 5) {                                  // pumped 5 times, need to take a break
    takeReadings = 0;
    flashingLights();
    if (pausePump == 0) {
      timeStamp = millis();
      pausePump = 1;    
    }
    if ((currentMillis - timeStamp) >= resetTime) {     // delay to reprocess - based on debug status
      pausePump = 0;                                     // reset trigger to start process all over
      tooManyPumpsDistance = sonar.ping_cm();
      if (tooManyPumpsDistance >= 20) {
        takeReadings = 1;
        pumpCount = 0;
      }
    }
  } else {
    //takeReadings = 1;
  }
}

void debugPrint() {
  if (debugOn == true) {
    Serial.print("takeReadings: ");
    Serial.println(takeReadings);
    Serial.print("waitingForHand: ");
    Serial.print(waitingForHand);
    Serial.print(" prepPump: ");
    Serial.print(prepPump);
    Serial.print(" pumpSanitizer: ");
    Serial.println(pumpSanitizer);
    Serial.print("initialHandSense: ");
    Serial.println(initialHandSense);
    Serial.print("pumpCount: ");
    Serial.println(pumpCount);
    if (superDebugOn == true) {
      Serial.println("***superDebug***");
      Serial.print("distance: ");
      Serial.print(sonar.ping_cm());
      Serial.println(" cm");
      Serial.print("tooManyPumpsDistance: ");
      Serial.print(sonar.ping_cm());
      Serial.println(" cm");
      Serial.print("currentMillis: ");
      Serial.println(currentMillis);
      Serial.print("timeStamp: ");
      Serial.println(timeStamp);
    }
    delay(1000);                                        // delay in the print
  }
}

void loop() {
  currentMillis = millis();
  if (takeReadings == 1) {
    distance = (sonar.ping_cm());
    if (distance >= 21) {
      waitingForHand = 1;
      prepPump = 0;
      pumpSanitizer = 0;    
      initialHandSense = 0;
      pumpCount = 0;
    } else if (distance <= 20) {
      waitingForHand = 0;
      prepPump = 1;
      pumpSanitizer = 0;
      if (initialHandSense == 0) {
        initialHandSense = 1;    
        timeStamp = millis();
      } else {
        
      }
    }
    if ((prepPump == 1) && (initialHandSense == 1) && ((currentMillis - timeStamp) > holdTime)) {
      waitingForHand = 0;
      prepPump = 0;
      pumpSanitizer = 1;    
      initialHandSense = 0;
    }
    outputControl();
  } else {
  }

  delay(50);
  debugPrint();
  tooManyPumps(); 
}
