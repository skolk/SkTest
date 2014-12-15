#include <stdio.h>
#include "auxbox_definitions.h"
#include "utils.h"
#include <AccelStepper.h>
#include "FlowSensor.h"
#include "FocusStepper.h"
#include "BathPath.h"
#include "BathTemp.h"

// Setup & Loop

void setup() {
  Serial.begin(9600);

  setupBathPath();
  setupFocusStepper();
  setupFlowMeter();
}

#define REQUEST_LENGTH 5
#define END_LINE 10 // byte value of '\n'
#define COMMAND_TIMEOUT_MS 200

// strncmp will return 0 if strings match; convert to 1/true
// If length isn't passed in, then examine the entire request
boolean cmdEqual(char *request, char *value, int length) {
  return ! strncmp(request, value, length);
}
boolean cmdEqual(char *request, char *value) {
  return cmdEqual(request, value, REQUEST_LENGTH - 1);
 
  
}

long firstCharTime;
boolean reading = false;

void loop() {
  int numBytesAvailable = Serial.available();

  if (numBytesAvailable >= REQUEST_LENGTH) {

    char request[REQUEST_LENGTH];
    for (int byteIndex = 0; byteIndex < REQUEST_LENGTH; byteIndex ++) {
      request[byteIndex] = Serial.read();
    }

    if (request[REQUEST_LENGTH - 1] == END_LINE) {
      processRequest(request);
    } else {
      sendError(ERROR_MALFORMED_COMMAND);
    }
    reading = false;

  } else if (! reading && numBytesAvailable > 0) {

    reading = true;
    firstCharTime = millis();

  } else if (reading && millis() - firstCharTime > COMMAND_TIMEOUT_MS) {

    while (Serial.available()) {
      Serial.read();
    }
    reading = false;

  }
}

void processRequest(char* request) {
  ERROR_STATE status = OK;

  // Read in the request.  Will be all null if does not end with /n
  if (cmdEqual(request, "PING")) {
    Serial.print("PING:v1.0\n");
  }

  else if (cmdEqual(request, "TEMP")) {
    float temperature = readTemp(&status);
    if (status != OK) {
      sendError(status);
      return;
    }
    Serial.print("TEMP:");
    sendFloat(temperature);
  }

  else if (cmdEqual(request, "FLOW")) {
    float flow = readFlow();
    Serial.print("FLOW:");
    sendFloat(flow);
  }

  else if (cmdEqual(request, "PATH")) {
    sendPath();
  }

  else if (cmdEqual(request, "PTH", 3)) {
    // This command looks like "PTH0", "PTH1", etc.
    // Use this ASCII trick to parse an int from 0 to 9 from
    // the last byte of request.
    int pathNum = request[3] - '0';
    processPathRequest(pathNum);
  }

  else if (cmdEqual(request, "NSTP")) {
    sendPosition();
  }

  else if (cmdEqual(request, "ZERO")) {
    stepper.setCurrentPosition(0);
    sendPosition();
  }
  /////////////////////////////////////////////////////////////////////
  
  //NO CAPTURE 
  else if (cmdEqual(request, "NCAP")) {

    valve1.close();
    valve2.close();
    valve3.open();
    digitalWrite(Silnoid, LOW);
    Serial.print("NCAP:DERP\n");
  }
  
//CAPTURE FLOW
  else if (cmdEqual(request, "CAPT")) {
    valve1.open();
    valve2.open();
    valve3.close();
    digitalWrite(Silnoid, LOW);
    Serial.print("CAPT:DERP");
  }
  
//KILL/END/ ALL SIGNALS ( SO RELAY ISN'T RUNNING) 
  else if (cmdEqual(request, "HOLD")) {
    valve1.hold();
    valve2.hold();
    valve3.hold();
    digitalWrite(Silnoid, HIGH);
    Serial.print("HOLD:DEPR");
  }
  

  else if (cmdEqual(request, "1OPN")) {
    valve1.open();
    Serial.print("1OPN:DERP");
  }
  
  else if (cmdEqual(request, "2OPN")) {
    valve2.open();
    Serial.print("2OPN:DERP");
  }

  else if (cmdEqual(request, "3OPN")) {
    valve3.open();
    Serial.print("3OPN:DERP");
  }
  
  else if (cmdEqual(request, "1COS")) {
    valve1.close();
      Serial.print("1COS:DERP");
  }
  
  else if (cmdEqual(request, "2COS")) {
    valve2.close();
    Serial.print("2COS:DERP");
  }
  
  
  else if (cmdEqual(request, "3COS")) {
    valve3.close();
    Serial.print("3COS:DERP");
  }
  
//START - OPEN SILNOID
  else if (cmdEqual(request, "STRT")) {
    digitalWrite(Silnoid, LOW);
    Serial.print("STRT:DERP");
  }

//STOP - CLOSE SILNOID 
  else if (cmdEqual(request, "STOP")) {
    digitalWrite(Silnoid, HIGH);
    Serial.print("STOP:DERP");
  }

//pump on
   else if (cmdEqual(request, "PPON")) {
    digitalWrite(PUMP, LOW);
      Serial.print("PPON:DERP");
      return;
  }
  
  //pump off
 else if (cmdEqual(request, "POFF")) {
    digitalWrite(PUMP, HIGH);
      Serial.print("POFF:DERP");
      return;
  }


   //heater on
 //else if (cmdEqual(request, "HEAT")) {
   // digitalWrite(PUMP, LOW);
     // Serial.print("HEAT ON, Captain");
      //return;
 // }

   //heater off
 //else if (cmdEqual(request, "HOFF")) {
    //digitalWrite(HEAT, HIGH);
      //Serial.print("HEAT OFF Captain");
      //return;
//  }
  
  ////////////////////////////////////////////////////////////////////////

  else if (cmdEqual(request, "HOME")) {
    status = runHoming();
    if (status != OK) {
      sendError(status);
      return;
    }
    sendPosition();
  }

  else if (cmdEqual(request, "TSTP")) {
    long newPosition = stepper.currentPosition() + 5000;
    status = moveSafelyToTarget(newPosition);
    if (status != OK) {
      sendError(status);
      return;
    }
    sendPosition();
  }

  else if (cmdEqual(request, "SWHM")) {
    Serial.print("SWHM:");
    sendFloat((float) digitalRead(SWITCH_PINS[SWITCH_HOMING]));
  }

  else if (cmdEqual(request, "SWSF")) {
    Serial.print("SWSF:");
    sendFloat((float) digitalRead(SWITCH_PINS[SWITCH_SAFETY]));
  }

  else if (cmdEqual(request, "TSTN")) {
    long newPosition = stepper.currentPosition() - 5000;
    status = moveSafelyToTarget(newPosition);
    if (status != OK) {
      sendError(status);
      return;
    }
    sendPosition();
  }

  else if (cmdEqual(request, "GOST")) {
    long newPosition = stepper.currentPosition() + 1;
    status = moveSafelyToTarget(newPosition);
    if (status != OK) {
      sendError(status);
      return;
    }
    sendPosition();
  }

  // This has to be at the end so that we can have other commands that start with ST.
  // Steps absolute (unsigned)
  else if (cmdEqual(request, "ST", 2)) {
    char* requestParam = &request[2];
    int target = getUnsignedInt(requestParam);
    status = moveSafelyToTarget(target);
    if (status != OK) {
      sendError(status);
      return;
    }
    sendPosition();
  }

  else {
    sendError(ERROR_MALFORMED_COMMAND);
  }
}
