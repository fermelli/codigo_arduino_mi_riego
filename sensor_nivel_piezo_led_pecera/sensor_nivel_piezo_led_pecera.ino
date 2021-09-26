const byte pinTrigger = 8;
const byte pinEcho = 9;
const byte pinPiezo = 10;
const byte pinLedNivelPecera = 11;
long tiempo;
long distancia;
long distanciaMaxima = 21;
long distanciaMinima = 17;

void setup() {
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrigger, LOW);

  pinMode(pinPiezo, OUTPUT);
  pinMode(pinLedNivelPecera, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  digitalWrite(pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrigger, LOW);

  tiempo = pulseIn(pinEcho, HIGH);
  distancia = tiempo/59;

  Serial.println(distancia);
  if(distancia > distanciaMaxima || distancia < distanciaMinima) {
    if(distancia > distanciaMaxima) {
     tone(pinPiezo, 1000); 
    }
    if(distancia < distanciaMinima) {
      tone(pinPiezo, 2000);
    }
    digitalWrite(pinLedNivelPecera, HIGH);
  } else {
    noTone(pinPiezo);
    digitalWrite(pinLedNivelPecera, LOW);
  }
  delay(1000);
}
