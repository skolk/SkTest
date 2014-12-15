#include "Valve.h"

Valve::Valve(uint8_t pinOpen, uint_8 pinClose) {
  PIN_OPEN = pinOpen;
  PIN_CLOSE = pinClose;

  pinMode(PIN_OPEN, OUTPUT);
  pinMode(PIN_CLOSE, OUTPUT);

  hold();
}

void Valve::open() {
  digitalWrite(PIN_OPEN, LOW);
  digitalWrite(PIN_CLOSE, HIGH);
}

void Valve::close() {
  digitalWrite(PIN_OPEN, HIGH);
  digitalWrite(PIN_CLOSE, LOW);
}

// Prevent the valve from opening or closing.
void Valve::hold() {
  digitalWrite(PIN_OPEN, HIGH);
  digitalWrite(PIN_CLOSE, HIGH); 
}