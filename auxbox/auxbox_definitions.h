#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#define DEBUG_PRINT true // YOU MUST comment this out if we're in production.
//#define NO_SWITCHES_ATTACHED true // Comment this out if the limit switches are attached.

// PIN ASSIGNMENTS
#define PIN_HALL_SENSOR 5

#define PIN_THERMO_DO 12
#define PIN_THERMO_CS 11
#define PIN_THERMO_CLK 10

#define PIN_PATH_NORMAL 6
#define PIN_PATH_CAPTURE 7
#define PIN_PATH_DIVERT 8

#define PIN_MOTOR_DIR 52
#define PIN_MOTOR_STEP 50

#define PIN_SERVO 13

#define PIN_SWITCH_SAFETY 17
#define PIN_SWITCH_HOMING 14


#define  valve1open 22 //valve 1 open
#define  valve1close 24 //valve 1 close
#define  valve2open  26 //valve 2 open
#define  valve2close 28 //valve 2 close
#define  valve3open  30 //valve 3 open
#define  valve3close  32 //valve 3 close
#define  Silnoid  34 // silnoid open (mechanical default - closed)
#define  PUMP  8 // pump open (mechanical default - closed)




// ERROR STATES
typedef int ERROR_STATE;

#define OK 0
#define ERROR_UNKNOWN_ERROR 1
#define ERROR_SAFETY_LIMIT 2
#define ERROR_HOMING_LIMIT 3
#define ERROR_MALFORMED_COMMAND 4
#define ERROR_OUT_OF_RANGE 5
#define ERROR_PATH_TIMEOUT 6
#define ERROR_PATH_INVALID 7
#define ERROR_TEMP_FAULT 8
#define ERROR_FLOW_INVALID 9

// REQUESTS
char request[4];
int requestIndex = 0;

#define REQUEST_LENGTH 5
#define END_LINE 10

#define COMMAND_TIMEOUT_MS 200
