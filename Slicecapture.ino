

int pin3 = 3; //valve 1 open
int pin4 = 4; //valve 1 close
int pin5 = 5; //valve 2 open
int pin6 = 6; //valve 2 close
int pin7 = 7; //valve 3 open
int pin8 = 8; //valve 3 close
int pin9 = 9; // silnoid open (mechanical default - closed) 


void setup()
{pinMode(pin3, OUTPUT);
pinMode(pin4, OUTPUT);
pinMode(pin5, OUTPUT);
pinMode(pin6, OUTPUT);
pinMode(pin7, OUTPUT);
pinMode(pin8, OUTPUT);
pinMode(pin9, OUTPUT);

}
void loop()
{
digitalWrite(pin3, HIGH);
digitalWrite(pin4, HIGH);
digitalWrite(pin6, HIGH);
digitalWrite(pin8, HIGH);
}
