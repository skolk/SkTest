typedef int PATH_NUM;

// Each of these is an index in the pathStates array
#define PATH_NORMAL 0
#define PATH_CAPTURE 1
#define PATH_COLLECT 2
#define PATH_FLUSH 3
#define PATH_INVALID 4

const int NUM_PATH_PINS = 3; // the length of `PATH_PINS`
int PATH_PINS[] = {PIN_PATH_NORMAL, PIN_PATH_CAPTURE, PIN_PATH_DIVERT};

const int NUM_PATH_STATES = 4; // the length of `pathStates`
int PATH_STATES[NUM_PATH_STATES][NUM_PATH_PINS] = {
  { LOW,  LOW,  LOW  }, // normal
  { HIGH, HIGH, LOW  }, // capture
  { LOW,  LOW,  LOW  }, // collect
  { HIGH, HIGH, HIGH }  // flush
};

// Set this to an impossible value unless we do have a request
PATH_NUM requestedPath = -1;

// pinValues is really an array, but we can't pass those :(
void writeToPathPins(int* pinValues)
{
  for (int i = 0; i < NUM_PATH_PINS; i++) {
    digitalWrite(PATH_PINS[i], pinValues[i]);
  }
}

// pinValues is really an array, but we can't pass those :(
void readFromPathPins(int* pinValues) {
  for (int i = 0; i < NUM_PATH_PINS; i++) {
    pinValues[i] = digitalRead(PATH_PINS[i]);
  }
}

// Change the path to newPath. Does not block on the path to finish changing.
ERROR_STATE setPath(PATH_NUM newPath)
{
  if (newPath >= PATH_INVALID || newPath < 0) {
    return ERROR_PATH_INVALID;
  }

  writeToPathPins(PATH_STATES[newPath]);
  return OK;
}

bool isPath(PATH_NUM pathNum, int* pinValues) {
  return memcmp(pinValues, PATH_STATES[pathNum], sizeof(int) * NUM_PATH_PINS) == 0;
}

// Read the input pins to see which path we're currently at.
PATH_NUM getPath()
{
  int pinValues[NUM_PATH_PINS];

  for (int i = 0; i < NUM_PATH_PINS; i++) {
    pinValues[i] = digitalRead(PATH_PINS[i]);
  }

  for (int i = 0; i < NUM_PATH_STATES; i++) {
    if (isPath(i, pinValues)) {
      return (PATH_NUM) i; // cast it to the right return type
    }
  }

  return PATH_INVALID;
}

// Poll the path until it reaches the setpoint.
ERROR_STATE blockOnPath(PATH_NUM pathSetPoint, long timeout = 1000) {
  long startTime = millis();
  int pinValues[NUM_PATH_PINS];
  while (millis() - startTime < timeout) {
    readFromPathPins(pinValues);
    if (isPath(pathSetPoint, pinValues)) {
      return OK;
    }
    delay(1);
  }

  return ERROR_PATH_TIMEOUT;
}

void sendPath() {
  int pinValues[NUM_PATH_PINS];
  readFromPathPins(pinValues);

  float pathToSend;

  if (requestedPath != -1 && isPath(requestedPath, pinValues)) {
    pathToSend = requestedPath;
  } else {
    pathToSend = getPath();
  }

  Serial.print("PATH:");
  sendFloat(pathToSend);
}

void processPathRequest(PATH_NUM pathNum) {
  requestedPath = pathNum;
  
  ERROR_STATE status = setPath(pathNum);
  if (OK != status) {
    sendError(status);
    return;
  }
  status = blockOnPath(pathNum);
  if (OK != status) {
    sendError(status);
    return;
  }
  sendPath();
}

void setupBathPath() {
  pinMode(PIN_HALL_SENSOR, INPUT);
  pinMode(PIN_PATH_NORMAL, OUTPUT);
  pinMode(PIN_PATH_CAPTURE, OUTPUT);
  pinMode(PIN_PATH_DIVERT, OUTPUT);
  
pinMode(valve1open, OUTPUT);
pinMode(valve1close, OUTPUT);
pinMode(valve2open, OUTPUT);
pinMode(valve2close, OUTPUT);
pinMode(valve3open, OUTPUT);
pinMode(valve3close, OUTPUT);
pinMode(Silnoid, OUTPUT);
pinMode(PUMP, OUTPUT); 

digitalWrite(valve1open, HIGH);
digitalWrite(valve1close, HIGH);
digitalWrite(valve2open, HIGH);
digitalWrite(valve2close, HIGH);
digitalWrite(valve3open, HIGH);
digitalWrite(valve3close, HIGH);
digitalWrite(Silnoid, HIGH);
digitalWrite(PUMP, HIGH);
  
  
}
