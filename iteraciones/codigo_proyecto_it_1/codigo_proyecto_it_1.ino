#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const float pinSHS1 = A0;
const float pinSHS2 = A1;

const byte pinReleIN2 = 41;
const byte pinReleIN3 = 42;

const int mumeroLecturasParaObtenerPromedio = 10;
const int tiempoEntreLecturasDelPromedio = 500;
const long tiempoEntreLecturasRealizadas = 5000;

float promedioPorcentajesSHS1 = 0;
float promedioPorcentajesSHS2 = 0;

const int porcentajeMinimoSHS1 = 0;
const int porcentajeMaximoSHS1 = 70;

const int porcentajeMinimoSHS2 = 0;
const int porcentajeMaximoSHS2 = 70;

const int tiempoRiegoSHS1 = 2000;
const int tiempoRiegoSHS2 = 2000;

void setup()
{
   Serial.begin(9600);
   lcd.init();
   lcd.backlight();

   pinMode(pinReleIN2, OUTPUT);
   pinMode(pinReleIN3, OUTPUT);

   digitalWrite(pinReleIN2, HIGH);
   digitalWrite(pinReleIN3, HIGH);
}

void loop()
{
   lecturasSensores(mumeroLecturasParaObtenerPromedio);

   if (promedioPorcentajesSHS1 >= porcentajeMinimoSHS1 && promedioPorcentajesSHS1 <= porcentajeMaximoSHS1)
   {
      digitalWrite(pinReleIN2, LOW);
      lcd.setCursor(2, 0);
      lcd.print("BOMBA AGUA 1");
      lcd.setCursor(2, 1);
      lcd.print("ACTIVANDO " + String(tiempoRiegoSHS1 / 1000) + "s");
      delay(tiempoRiegoSHS1);
      digitalWrite(pinReleIN2, HIGH);
   }

   if (promedioPorcentajesSHS2 >= porcentajeMinimoSHS2 && promedioPorcentajesSHS2 <= porcentajeMaximoSHS2)
   {
      digitalWrite(pinReleIN3, LOW);
      lcd.setCursor(2, 0);
      lcd.print("BOMBA AGUA 2");
      lcd.setCursor(2, 1);
      lcd.print("ACTIVANDO " + String(tiempoRiegoSHS2 / 1000) + "s");
      delay(tiempoRiegoSHS2);
      digitalWrite(pinReleIN3, HIGH);
   }
}

void lecturasSensores(int numeroLecturas)
{
   lcd.clear();
   lcd.setCursor(4, 0);
   lcd.print("LEYENDO");
   lcd.setCursor(2, 1);
   lcd.print("SENSORES...");

   float sumaPorcentajesSHS1 = 0;
   float sumaPorcentajesSHS2 = 0;
   for (int i = 1; i <= numeroLecturas; i++)
   {
      float lecturaSHS1 = analogRead(pinSHS1);
      float lecturaSHS2 = analogRead(pinSHS2);

      float porcentajeSHS1 = map(lecturaSHS1, 1021, 305, 0, 100);
      float porcentajeSHS2 = map(lecturaSHS2, 1022, 275, 0, 100);

      sumaPorcentajesSHS1 = sumaPorcentajesSHS1 + porcentajeSHS1;
      sumaPorcentajesSHS2 = sumaPorcentajesSHS2 + porcentajeSHS2;

      delay(tiempoEntreLecturasDelPromedio);
   }

   promedioPorcentajesSHS1 = sumaPorcentajesSHS1 / numeroLecturas;
   promedioPorcentajesSHS2 = sumaPorcentajesSHS2 / numeroLecturas;

   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("HUMEDAD SUELO");
   lcd.setCursor(0, 1);
   lcd.print("S1:" + String(int(promedioPorcentajesSHS1)) + "%");
   lcd.setCursor(9, 1);
   lcd.print("S2:" + String(int(promedioPorcentajesSHS2)) + "%");
   delay(tiempoEntreLecturasRealizadas);
   lcd.clear();
}
