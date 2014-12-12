

int valve1open = 22; //valve 1 open
int valve9close = 24; //valve 1 close
int valve5open = 26; //valve 2 open
int valve2close = 28; //valve 2 close
int valve3open = 30; //valve 3 open
int valve3close = 32; //valve 3 close
int Silnoid = 34; // silnoid open (mechanical default - closed) 

flow(capt) 
flow(nocap)
flow(stop)
flow(start) 

void setup()
{pinMode(valve1open, OUTPUT);
pinMode(valve1close, OUTPUT);
pinMode(valve2open, OUTPUT);
pinMode(valve2close, OUTPUT);
pinMode(valve3open, OUTPUT);
pinMode(valve3close, OUTPUT);
pinMode(Silnoid, OUTPUT);

}


void loop()
{
digitalWrite(valve1open, HIGH);
digitalWrite(valve1close, HIGH);
digitalWrite(valve2close, HIGH);
digitalWrite(valve3close, HIGH);
}



  else if (cmdEqual(request, "CAPT")) {
    digitalWrite(valve1open, HIGH);
    digitalWrite(valve2open, HIGH);
    digitalWrite(valve3close, HIGH);
    digitalWrite(Silnoid, HIGH);
  }

    else if (cmdEqual(request, "NOCAP")) {
    digitalWrite(valve1close, HIGH);
    digitalWrite(valve2close, HIGH);
    digitalWrite(valve3open, HIGH);
    digitalWrite(Silnoid, HIGH);
  }

    else if (cmdEqual(request, "STOP")) {
    digitalWrite(Silnoid, LOW);
  }

      else if (cmdEqual(request, "START")) {
    digitalWrite(Silnoid, HIGH);
  }