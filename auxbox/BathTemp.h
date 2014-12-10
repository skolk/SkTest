#include "Adafruit_MAX31855.h"

Adafruit_MAX31855::Adafruit_MAX31855 thermocouple(PIN_THERMO_CLK, PIN_THERMO_CS, PIN_THERMO_DO);

float readTemp(ERROR_STATE *status) {
  float tempInC = thermocouple.readCelsius();

  // the value will be NAN if the temperature sensor
  // returns a fault, but if there's no sensor hooked up,
  // the call will return 32 bits of zeros. In floating point,
  // all zeros corresponds to positive zero.
  if (isnan(tempInC) || tempInC == +0.0) {
    *status = ERROR_TEMP_FAULT;
  } else {
    *status = OK;
  }
  return tempInC;
}
