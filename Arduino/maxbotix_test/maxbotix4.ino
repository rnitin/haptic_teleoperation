const int pwPin = 10;
long d = 0;

void setup() 
{
  Serial.begin(115200);
  pinMode(pwPin, INPUT);
}

void read_sensor ()
{
  d = pulseIn(pwPin, HIGH)/58;
  if (d > 200)
    d = 200;
  Serial.println(d);  
}

void loop() 
{ 
  read_sensor();
  delay(100);
}
