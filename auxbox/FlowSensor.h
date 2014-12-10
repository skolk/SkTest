#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

volatile int RisingEdge = 0;
int FlowArray[60];
float AverageFlow;

float readFlow(void);
void incrementRisingEdge(void);
void setupFlowMeter(void);

long lastMeasurementTimeMS = millis();

float readFlow() {
  int flowTicks = RisingEdge;
  RisingEdge = 0;

  long now = millis();
  float timeSinceLastMeasurementMS = (float)(now - lastMeasurementTimeMS);
  lastMeasurementTimeMS = now;

  const float timeFactor = 1000.0 * 60.0 / 7.5; //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
  return flowTicks * timeFactor / timeSinceLastMeasurementMS;
}

void incrementRisingEdge() { // Increment RisingEdge every interrupt, when signal rises
  RisingEdge++;
}

void setupFlowMeter() {
  attachInterrupt(0, incrementRisingEdge, RISING); //the interrupt is attached
}