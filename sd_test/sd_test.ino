#include <SPI.h>
#include <SD.h>

File miArchivo;

#define interruptor 5
#define led 7

String nombreArchivo = "data.csv";

void setup()
{
  pinMode(interruptor, INPUT);
  pinMode(led, OUTPUT);

  Serial.begin(9600);
  while (!Serial)
  {
    ; // espera que se conecte el puerto serial (por USB)
  }

  Serial.println("Inicializando tarjeta SD...");

  if (!SD.begin(4))
  {
    Serial.println("¡Inicializacion fallida!");
    while (1)
      ;
  }
  Serial.println("Inicializacion completa");
}

void loop()
{
  byte lectura = digitalRead(interruptor);
  if (lectura == HIGH)
  {

    miArchivo = SD.open(nombreArchivo, FILE_WRITE);

    if (miArchivo)
    {
      Serial.println("escribiendo en " + nombreArchivo);
      miArchivo.println("probando,123,true,1.54");
      miArchivo.close();

      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);

      Serial.println("¡Hecho!");
    }
    else
    {
      Serial.println("¡Error abriendo " + nombreArchivo);
    }
  }
}