
unsigned char dta[100];
unsigned char len = 0;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    while(Serial.available()>=9)
    {
        if((0x59 == Serial.read()) && (0x59 == Serial.read())) //Byte1 & Byte2
        {
            unsigned int t1 = Serial.read(); //Byte3
            unsigned int t2 = Serial.read(); //Byte4

            t2 <<= 8;
            t2 += t1;
            Serial.print(t2);
            Serial.print('\t');

            t1 = Serial.read(); //Byte5
            t2 = Serial.read(); //Byte6

            t2 <<= 8;
            t2 += t1;
            Serial.println(t2);

            for(int i=0; i<3; i++) 
            { 
                Serial.read(); ////Byte7,8,9
            }
        }
    }
}
