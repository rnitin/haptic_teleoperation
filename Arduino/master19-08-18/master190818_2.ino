const int pwPin1 = 10, pwPin2 = 6, pwPin3 = 5, pwPin4 = 9;
long d1, d2, d3, d4, op1 = 0, op2 = 0;

void setup() 
{
  Serial.begin(115200);
  pinMode(pwPin1, INPUT);
  pinMode(pwPin2, INPUT);
  pinMode(pwPin3, INPUT);
  pinMode(pwPin4, INPUT);
}

void read_sensor ()
{
  d1 = pulseIn(pwPin1, HIGH)/58;
  d2 = pulseIn(pwPin2, HIGH)/58;
  d3 = pulseIn(pwPin3, HIGH)/58;
  d4 = pulseIn(pwPin4, HIGH)/58;
  if (d1 > 200)
    d1 = 200;
  if (d2 > 200)
    d2 = 200;
  if (d3 > 200)
    d3 = 200;
  if (d4 > 200)
    d4 = 200;
}

void print_range()
{
  /*op1 = 3*1000000 + d1*1000 + d2;
  Serial.println(op1);
  op2 = 4*1000000 + d3*1000 + d4;
  Serial.println(op2);
  */
  Serial.print(d1);
  Serial.print('\t');
  Serial.print(d2);
  Serial.print('\t');
  Serial.print(d3);
  Serial.print('\t');
  Serial.print(d4);
  Serial.print('\n');
  
}

void loop() 
{ 
  read_sensor();
  /*print_range();
  delay(20);
  print_range();
  delay(20);
  print_range();
  delay(20);
  print_range();
  delay(20);
  */
  print_range();
  //delay(80);
}
