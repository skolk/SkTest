
int dir = 50;        // direction pin
int steppin = 52;      // step pin
int Distance = 0;  // Record the number of steps we've taken
int stepdelay = 100; // number of microseconds to delay

void setup() {                
  pinMode(dir, OUTPUT);     
  pinMode(steppin, OUTPUT);
  digitalWrite(dir, LOW);
  digitalWrite(steppin, LOW);
  
  
  Serial.begin(9600);
  Serial.println('init');
}

void loop() {
  if (Serial.available() > 0)
  {
    int steps = Serial.parseInt();
    Serial.println(steps);
    
    if (steps > 0){
      for (int cnt = 0; cnt < steps; cnt++) {
        digitalWrite(steppin, HIGH);
        delayMicroseconds(stepdelay);          
        digitalWrite(steppin, LOW); 
        delayMicroseconds(stepdelay);
        Distance = Distance + 1;   // record this step
        }
      Serial.println(Distance);
    }
    if (steps < 0){
      digitalWrite(dir, HIGH);
      delay(stepdelay);
      for (int cnt = 0; cnt < abs(steps); cnt++) {
        digitalWrite(step, HIGH);
        delayMicroseconds(stepdelay);          
        digitalWrite(step, LOW); 
        delayMicroseconds(stepdelay);
        Distance = Distance - 1;   // record this step
      }
      digitalWrite(dir, LOW);
      Serial.println(Distance);
    }
  }
}
