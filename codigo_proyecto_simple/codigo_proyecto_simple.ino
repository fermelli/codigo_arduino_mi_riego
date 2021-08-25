// Librerias para el uso de la pantalla LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Variable para el manejo de la pantalla LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

/* 
   Pines para los Sensores de Humedad de Suelo
   Los pines para los sensores de humedad de suelo deben ser analógicos
   SHS1 -> Sensor de Humedad de Suelo 1
   SHS2 -> Sensor de Humedad de Suelo 2
*/
const float pinSHS1 = A0;
const float pinSHS2 = A1;

/*
   La placa Arduino detectará el nivel de voltaje entregado por el sensor de humedad de suelo,
   y lo convertirá en un número equivalente a la cantidad de humedad detectada entre 0 y 1023
*/
const float valorLecturaTierraSeca = 1023; // Constante para el valor de la tierra seca

/* 
   PINES PARA LOS RELES
   Se deben conectar en pines digitales de la placa, por lo tanto la funcion digitalWrite(pin, ESTADO) donde:
   pin: es el numero del pin de la entrada (IN) del rele
   ESTADO: puede ser LOW (Encendido) o HIGH (Apagado)
*/
const byte pinReleIN1 = 40; // asociada a la bomba de agua (recirculacion)
const byte pinReleIN2 = 41; // asociada a la bomba de agua (riego del SHS1)
const byte pinReleIN3 = 42; // asociada a la bomba de agua (riego del SHS2)
const byte pinLed = 7;      // como aviso para cuando la bomba de agua (recirculacion) esta encendedida

// Interruptor
const byte pinInterruptor = 38;
bool encendidoBombaAguaRecirculacion = false;

// Numero de lecturas del sensor de humedad para el promedio
const byte mumeroLecturasParaObtenerPromedio = 10;
// tiempo entre cada una de las lecturas
const int tiempoEntreLecturas = 500;
// tiempo para mostrar los resultados de la lectura
const int tiempoMostrandoLecturasHumedadSuelo = 5000;

// Variables para el promedio de humedades del suelo leidas por los sensores
float promedioLecturasSHS1 = 0;
float promedioLecturasSHS2 = 0;
float promedioPorcentajesSHS1 = 0;
float promedioPorcentajesSHS2 = 0;

// Valores minimos y maximos para encender la bomba de agua (riego del SHS1)
const int porcentajeMinimoSHS1 = 0;
const int porcentajeMaximoSHS1 = 70;

// Valores minimos y maximos para encender la bomba de agua (riego del SHS2)
const int porcentajeMinimoSHS2 = 0;
const int porcentajeMaximoSHS2 = 70;

// Tiempo de riego para la bomba de agua (riego del SHS1)
const int tiempoRiegoSHS1 = 2000;
// Tiempo de riego para la bomba de agua (riego del SHS2)
const int tiempoRiegoSHS2 = 2000;

void setup()
{
   lcd.init();      // inicializa la pantalla LCD
   lcd.backlight(); // enciende la luz de fondo del LCD
   // Establecemos el modo de los pines
   pinMode(pinReleIN1, OUTPUT);
   pinMode(pinReleIN2, OUTPUT);
   pinMode(pinReleIN3, OUTPUT);
   pinMode(pinInterruptor, INPUT_PULLUP);
   pinMode(pinLed, OUTPUT);
   // Bombas de agua apagadas
   digitalWrite(pinReleIN1, HIGH);
   digitalWrite(pinReleIN2, HIGH);
   digitalWrite(pinReleIN3, HIGH);
   // Inicializacion puerto serial
   Serial.begin(9600);
}

void loop()
{
   promedioHumedadesSuelo(mumeroLecturasParaObtenerPromedio, valorLecturaTierraSeca);

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

   int lectura = digitalRead(pinInterruptor);
   if (lectura == HIGH)
   {
      encendidoBombaAguaRecirculacion = true;
   }
   else
   {
      encendidoBombaAguaRecirculacion = false;
   }

   if (encendidoBombaAguaRecirculacion)
   {
      Serial.println("Bomba de agua de recirculacion encendida");
      digitalWrite(pinReleIN1, LOW);
      digitalWrite(pinLed, HIGH);
   }
   else
   {
      Serial.println("Bomba de agua de recirculacion apagada");
      digitalWrite(pinReleIN1, HIGH);
      digitalWrite(pinLed, LOW);
   }
}

void promedioHumedadesSuelo(byte numeroLecturas, float valorLecturaTierraSeca)
{
   lcd.clear();
   lcd.setCursor(4, 0);
   lcd.print("LEYENDO");
   lcd.setCursor(2, 1);
   lcd.print("HUMEDADES...");
   float sumaPorcentajesSHS1 = 0;
   float sumaPorcentajesSHS2 = 0;
   float sumaLecturasSHS1 = 0;
   float sumaLecturasSHS2 = 0;
   for (int i = 1; i <= numeroLecturas; i++)
   {
      float lecturaSHS1 = analogRead(pinSHS1);
      float lecturaSHS2 = analogRead(pinSHS2);
      sumaLecturasSHS1 = sumaLecturasSHS1 + lecturaSHS1;
      sumaLecturasSHS2 = sumaLecturasSHS2 + lecturaSHS2;
      float porcentajeSHS1 = map(lecturaSHS1, 1021, 305, 0, 100);
      float porcentajeSHS2 = map(lecturaSHS2, 1022, 275, 0, 100);
      // float porcentajeSHS1 = (100 - ((lecturaSHS1 * 100) / valorLecturaTierraSeca)) * 2;
      // float porcentajeSHS2 = (100 - ((lecturaSHS2 * 100) / valorLecturaTierraSeca)) * 2;
      sumaPorcentajesSHS1 = sumaPorcentajesSHS1 + porcentajeSHS1;
      sumaPorcentajesSHS2 = sumaPorcentajesSHS2 + porcentajeSHS2;
      delay(tiempoEntreLecturas);
   }
   promedioLecturasSHS1 = sumaLecturasSHS1 / numeroLecturas;
   promedioLecturasSHS2 = sumaLecturasSHS2 / numeroLecturas;
   promedioPorcentajesSHS1 = sumaPorcentajesSHS1 / numeroLecturas;
   promedioPorcentajesSHS2 = sumaPorcentajesSHS2 / numeroLecturas;
   Serial.println("S1: " + String(promedioLecturasSHS1));
   Serial.println("S2: " + String(promedioLecturasSHS2));
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("LECTURA HUMEDAD");
   lcd.setCursor(0, 1);
   lcd.print("S1:" + String(int(promedioPorcentajesSHS1)) + "%");
   lcd.setCursor(9, 1);
   lcd.print("S2:" + String(int(promedioPorcentajesSHS2)) + "%");
   delay(tiempoMostrandoLecturasHumedadSuelo);
   lcd.clear();
}
