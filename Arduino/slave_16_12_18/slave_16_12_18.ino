int i,n;

void setup()
{
  Serial.begin(9600);
}

void loop() 
{
  if(Serial.available())
  {
    i = Serial.parseInt();
      Serial.println(i);
  }
  /*else
  {
    Serial.println(100);
    delay(15);
  }*/
}