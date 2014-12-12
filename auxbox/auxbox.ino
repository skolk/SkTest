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
    digitalWrite(valve1open, HIGH);
    digitalWrite(valve1close, LOW);
    digitalWrite(valve2open, HIGH);
    digitalWrite(valve2close, LOW);
    digitalWrite(valve3open, LOW);
    digitalWrite(valve3close, HIGH);
    digitalWrite(Silnoid, LOW);
      Serial.print("NCAP Captain");
  }
  
//CAPTURE FLOW
  else if (cmdEqual(request, "CAPT")) {
    digitalWrite(valve1open, LOW);
    digitalWrite(valve1close, HIGH);
    digitalWrite(valve2open, LOW);
    digitalWrite(valve2close, HIGH);
    digitalWrite(valve3open, HIGH);
    digitalWrite(valve3close, LOW);
    digitalWrite(Silnoid, LOW);
      Serial.print("CAPT Captain");
  }
  
//KILL/END/ ALL SIGNALS ( SO RELAY ISN'T RUNNING) 
  else if (cmdEqual(request, "KILL")) {
    digitalWrite(valve1open, HIGH);
    digitalWrite(valve1close, HIGH);
    digitalWrite(valve2open, HIGH);
    digitalWrite(valve2close, HIGH);
    digitalWrite(valve3open, HIGH);
    digitalWrite(valve3close, HIGH);
    digitalWrite(Silnoid, HIGH);
      Serial.print("KILL Captain");
  }
  

  else if (cmdEqual(request, "1OPN")) {
    digitalWrite(valve1open, LOW);
    digitalWrite(valve1close, HIGH);
      Serial.print("1OPN Captain");
  }
  
    else if (cmdEqual(request, "2OPN")) {
    digitalWrite(valve2open, LOW);
    digitalWrite(valve2close, HIGH);
      Serial.print("2OPN Captain");
  }

    else if (cmdEqual(request, "3OPN")) {
    digitalWrite(valve3open, LOW);
    digitalWrite(valve3close, HIGH);
      Serial.print("3OPN Captain");
  }
  
    else if (cmdEqual(request, "1COS")) {
    digitalWrite(valve1open, LOW);
    digitalWrite(valve1close, HIGH);
      Serial.print("1COS Captain");
  }
  
      else if (cmdEqual(request, "2COS")) {
    digitalWrite(valve2open, HIGH);
    digitalWrite(valve2close, LOW);
      Serial.print("2COS Captain");
  }
  
  
        else if (cmdEqual(request, "3COS")) {
    digitalWrite(valve3open, LOW);
    digitalWrite(valve3close, HIGH);
      Serial.print("3COS Captain");
  }
  
//START - OPEN SILNOID
  else if (cmdEqual(request, "STRT")) {
    digitalWrite(Silnoid, LOW);
    Serial.print("FLowing Freely Captain");
  }

//STOP - CLOSE SILNOID 
  else if (cmdEqual(request, "STOP")) {
    digitalWrite(Silnoid, HIGH);
    Serial.print("Stop'n Captain");
  }

//pump on
   else if (cmdEqual(request, "PPON")) {
    digitalWrite(PUMP, LOW);
      Serial.print("PUMP ON Captain");
      return;
  }
  
  //pump off
 else if (cmdEqual(request, "POFF")) {
    digitalWrite(PUMP, HIGH);
      Serial.print("PUMP OFF Captain");
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
