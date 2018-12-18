int sensorPin = A0;    
int sensorValue = 0; 

void setup() 
{
  pinMode(sensorPin, INPUT);
  Serial.begin(115200);
}

void loop() 
{
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  delay(100);
}
