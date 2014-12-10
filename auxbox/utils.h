#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

byte getUnsignedInt(char*);
boolean cmdEqual(char*, char*);
void sendFloat(float);
void sendError(ERROR_STATE);

// convert target from bytes to int
byte getUnsignedInt(char* command) {
  byte* cmd = (byte*) command;
  unsigned int nsteps =  cmd[0] | (cmd[1] << 8);
  return nsteps;
}

#ifdef DEBUG_PRINT
void sendFloat(float f) {
  Serial.print(f);
  Serial.print('\n');
}
#else
void sendFloat(float f) {
  unsigned char *bytes = (unsigned char *) &f;
  int numBytesWritten = Serial.write(bytes, 4);
  if (numBytesWritten != 4) {
    // Note: potential infinite recursion
    sendError(ERROR_UNKNOWN_ERROR);
    return;
  }
  Serial.print('\n');
}
#endif

void sendError(ERROR_STATE errNum = ERROR_UNKNOWN_ERROR) {
  Serial.print("ERRR:");
  sendFloat((float) errNum);
}

/*
1. arduino timeout 
2. clear read buffer on both sides
3. on system startup, ensure arduino is in 'ready and waiting' state before proceeding, i.e.,
wait out the arduino timeout before starting to send stuff
*/