#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

RTC_DS3231 reloj;

File miArchivo;

#define interruptor 5
#define led 7

long porcentajeHumedad;

String nombreArchivo = "data.csv";

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

  //  while (!Serial) {
  //    ; // espera que se conecte el puerto serial (por USB)
  //  }

  Serial.println("Inicializando tarjeta SD...");

  if (!SD.begin(4))
  {
    Serial.println("¡Inicializacion fallida!");
    while (1)
      ;
  }
  Serial.println("Inicializacion completa");

  //  if(SD.exists(nombreArchivo)) {
  //    SD.remove(nombreArchivo);
  //  }
}

void loop()
{
  byte lectura = digitalRead(interruptor);
  if (lectura == HIGH)
  {

    miArchivo = SD.open(nombreArchivo, FILE_WRITE);

    if (miArchivo)
    {
      Serial.println("Escribiendo en " + nombreArchivo);
      DateTime ahora = reloj.now();
      porcentajeHumedad = random(50, 80);
      String dato = String(porcentajeHumedad) + "," + ahora.timestamp();
      miArchivo.println(dato);
      miArchivo.close();

      Serial.println(dato);

      digitalWrite(led, HIGH);
      delay(2000);
      digitalWrite(led, LOW);

      Serial.println("¡Hecho!");
    }
    else
    {
      Serial.println("¡Error abriendo " + nombreArchivo);
    }
  }
}