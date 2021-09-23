#define pinSensorNivelSuperior 4
#define pinSensorNivelInferior 3
#define pinInterruptor 8
#define pinLed 9

boolean aNivelMaximo;
boolean aNivelMinimo;
bool encendidoBombaAguaRecirculacion;

void setup()
{
  pinMode(pinSensorNivelSuperior, INPUT);
  pinMode(pinSensorNivelInferior, INPUT);
  pinMode(pinInterruptor, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);

  Serial.begin(9600);

  aNivelMaximo = false;
  aNivelMinimo = false;
  encendidoBombaAguaRecirculacion = false;
}

void loop()
{
  if (digitalRead(pinSensorNivelInferior) == HIGH)
  {
    aNivelMinimo = true;
  }
  else
  {
    aNivelMinimo = false;
  }

  if (digitalRead(pinSensorNivelSuperior) == HIGH)
  {
    aNivelMaximo = true;
  }
  else
  {
    aNivelMaximo = false;
  }

  if (aNivelMaximo && aNivelMinimo)
  {
    encendidoBombaAguaRecirculacion = true;
  }
  else
  {
    if (!aNivelMinimo)
    {
      encendidoBombaAguaRecirculacion = false;
    }
  }

  byte lecturaInterruptor = digitalRead(pinInterruptor);
  if (encendidoBombaAguaRecirculacion || lecturaInterruptor == HIGH)
  {
    //encender rele 1
    Serial.println("Bomba encendida");
    digitalWrite(pinLed, HIGH);
  }
  else
  {
    //apagar rele 2
    Serial.println("Bomba apagada");
    digitalWrite(pinLed, LOW);
  }
  delay(3000);
}
