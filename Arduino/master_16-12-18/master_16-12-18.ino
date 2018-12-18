#include <SoftwareSerial.h>

SoftwareSerial blSerial(11, 12); // RX, TX
const int pwPin1 = 10;
long d1, d2, op = 0, t1, t2;

void setup() 
{
  Serial.begin(115200);
  blSerial.begin(115200);
  pinMode(pwPin1, INPUT);  
}

void read_sensor ()
{
  d1 = pulseIn(pwPin1, HIGH)/58;
  //d1 = analogRead(A0);
  
/*
  while(tfSerial.available()>=9)
    {
        if((0x59 == tfSerial.read()) && (0x59 == tfSerial.read())) //Byte1 & Byte2
        {
            t1 = tfSerial.read(); //Byte3
            t2 = tfSerial.read(); //Byte4

            t2 <<= 8;
            t2 += t1;
            d2 = t2;
            
            for(int i=0; i<5; i++) 
            { 
                tfSerial.read(); ////Byte5,6,7,8,9
            }
        }
    }

*/
  while(Serial.available()>=9)
    {
        if((0x59 == Serial.read()) && (0x59 == Serial.read())) //Byte1 & Byte2
        {
            t1 = Serial.read(); //Byte3
            t2 = Serial.read(); //Byte4

            t2 <<= 8;
            t2 += t1;
            d2 = t2;
            
            for(int i=0; i<5; i++) 
            { 
                Serial.read(); ////Byte5,6,7,8,9
            }
        }
    }
  
  if (d1 > 300)
    d1 = 300;
  if (d2 > 300)
    d2 = 300;
}

void print_range()
{
  op = 4*1000000 + d1*1000 + d2;
  blSerial.println(op);
  
  Serial.print(d1);
  Serial.print('\t');
  Serial.print(d2);
  Serial.print('\n');
}

void loop() 
{ 
  read_sensor();
  print_range();
  delay(100);
}
