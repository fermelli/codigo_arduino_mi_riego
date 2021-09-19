int sensor = 3;

void setup()
{                
  pinMode(sensor,INPUT);
  Serial.begin(9600);
}
 
void loop()
{
  if(digitalRead(sensor) == LOW){
    Serial.println("Vacio");
  } else {
    Serial.println("Lleno");
  }
}
