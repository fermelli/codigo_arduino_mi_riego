#include "RTClib.h"

RTC_DS3231 reloj;

#define interruptor 5
#define led 7

long porcentajeHumedad;

void setup()
{
  pinMode(interruptor, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  if (!reloj.begin())
  {
    Serial.println("No se pudo encontrar el Reloj RTC");
    Serial.flush();
    abort();
  }

  if (reloj.lostPower())
  {
    Serial.println("¡El Reloj RTC perdió energía, ¡establezcamos la hora!");
    reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop()
{

  byte lectura = digitalRead(interruptor);
  if (lectura == HIGH)
  {
    DateTime ahora = reloj.now();
    porcentajeHumedad = random(50, 80);
    Serial.print(porcentajeHumedad);
    Serial.print(",");
    Serial.println(ahora.timestamp());
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
  }
}