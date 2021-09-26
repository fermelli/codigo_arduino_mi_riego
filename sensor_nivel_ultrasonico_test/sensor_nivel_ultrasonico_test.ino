const byte trigger = 8;
const byte echo = 9;

void setup() {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trigger, LOW);
  Serial.begin(9600);
}

void loop() {
  long tiempo;
  long distancia;

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  tiempo = pulseIn(echo, HIGH);
  distancia = tiempo/59;

  Serial.print("Distancia sensor: ");
  Serial.print(distancia);
  Serial.print("cm");
  Serial.println();
  delay(5000); 
}
