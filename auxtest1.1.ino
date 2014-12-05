
// trip senor setup 
int trip1 = 14;
int trip2 = 17;


//ph sensor 
#define SensorPin 0          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

//slice capture
int openvalve1 = 22;
int closevalve1 = 24;
int openvalve2 = 26;
int closevalve2 = 28;
int openvalve3 = 30;
int closevalve3 = 32;
int silnoid = 34;


//Adafruit tempsesor setup
#include "Adafruit_MAX31855.h"
int thermoDO = 12;
int thermoCS = 11;
int thermoCLK = 10;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// stepper setup 
int dir = 50;        // direction pin
int step = 52;      // step pin
int Distance = 0;  // Record the number of steps we've taken
int stepdelay = 100; // number of microseconds to delay


//flow meter 
#define FLOWSENSORPIN 2

/ count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!
SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}


void setup() {                
 // stepper setup 
  pinMode(dir, OUTPUT);     
  pinMode(step, OUTPUT);
  digitalWrite(dir, LOW);
  digitalWrite(step, LOW);
  
  
  Serial.begin(9600);
  Serial.println('init');

// flow sensor setup
  pinMode(FLOWSENSORPIN, INPUT);
  digitalWrite(FLOWSENSORPIN, HIGH);
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
  useInterrupt(true);

//phmeter setpup 
pinMode(13,OUTPUT)

}

void loop() {


	// stepper readout. 

  if (Serial.available() > 0)
  {
    int steps = Serial.parseInt();
    Serial.println(steps);
    
    if (steps > 0){
      for (int cnt = 0; cnt < steps; cnt++) {
        digitalWrite(step, HIGH);
        delayMicroseconds(stepdelay);          
        digitalWrite(step, LOW); 
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
// Temp readout 
	if serial read = "temp" then --->

	Serial.print("Internal Temp = ");
   Serial.println(thermocouple.readInternal());

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
     Serial.print("C = "); 
     Serial.println(c);


// Flow Readout 

if seral read = flow rate then _---> 

  Serial.print("Freq: "); Serial.println(flowrate);
  Serial.print("Pulses: "); Serial.println(pulses, DEC);
  
  // if a plastic sensor use the following calculation
  // Sensor Frequency (Hz) = 7.5 * Q (Liters/min)
  // Liters = Q * time elapsed (seconds) / 60 (seconds/minute)
  // Liters = (Frequency (Pulses/second) / 7.5) * time elapsed (seconds) / 60
  // Liters = Pulses / (7.5 * 60)
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;



// ph readout

if serial read = ph then -----> 

for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 


//slice captuer 

if serial read = open 1
if serial read = close 1 
if serial read = open 2
if serial read = close 2 
if serial read = open 3
if serial read = close 3
if serial read = open sinoid
if serial read = close sinoid 





}

boolean cmdEqual(char *request, char *value) {
  return !strncmp(request, value, 4);

void processRequest() {
  // Read in the request.  Will be all null if does not end with /n
  if (cmdEqual(request, "PING")) {
    Serial.println("PONG");
  }

  else if (cmdEqual(request, "TEMP")) {
    float temperature = readTemp();
    Serial.print("TEMP:");
    sendFloat(temperature);
  }

  else if (cmdEqual(request, "FLOW")) {
    Serial.print("FLOW:");
    sendFloat(AverageFlow);
  }

  else if (cmdEqual(request, "PATH")) {
    sendPath();
  }

  else if (!strncmp(request, "PTH", 3)) {
    int pathNum = request[3] - '0'; // ASCII trick to parse an int from 0 to 9
    processPathRequest(pathNum);
  }

  else if (cmdEqual(request, "OFFF")) {
    processPathRequest(PATH_ALL_OFF);
