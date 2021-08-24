//Librerias para el uso de la pantalla LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Variable para el manejo de la pantalla LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

//Pines para los sensores de humedad
const float pin_sh1 = A0;
const float pin_sh2 = A1;

//Constante para el valor de la tierra seca
const float seco = 1023;

/* 
   PINES PARA LOS RELES
   Se deben conectar en los pines digitales de la placa
*/
const int releIN1 = 40; // bomba de agua (recirculacion)
const int releIN2 = 41; // bomba de agua (riego)
const int releIN3 = 42; // bomba de agua (riego)

// Interruptor
const byte interruptorBombaRecirculacion = 38;
bool esta_encendida = false;

//Numero de lecturas del sensor de humedad para el promedio
const int cant_lec = 10;
//tiempo entre cada uno de las lecturas
const int time_lectura = 50;
//tiempo para mostrar los resultados de la lectura
const int time_mo = 5000;

//Variables del promedio de lectura de humedades
float prom_sh1 = 0;
float prom_sh2 = 0;

//Valores minimos y maximos para encender la bomba de la lechuga
const int min_le = 0;
const int max_le = 70;

//Valores minimos y maximos para encender la bomba de la cebolla
//const int min_ce = 0;
//const int max_ce = 70;

//Tiempo de riego para la lechuga
const int time_le = 2000;
//Tiempo de riego para la cebolla
//const int time_ce = 2000;

void setup()
{
   lcd.init();
   pinMode(releIN2, OUTPUT);
   pinMode(releIN3, OUTPUT);
   pinMode(releIN1, OUTPUT);
   pinMode(interruptorBombaRecirculacion, INPUT_PULLUP);
   Serial.begin(9600);
}

void loop()
{
   digitalWrite(releIN2, HIGH);
   digitalWrite(releIN3, HIGH);
   lcd.clear();
   lcd.backlight();
   lcd.setCursor(0, 0);
   lcd.print("LECTURA HUMEDAD");

   //Se llama a prodedimiento que promedia las lecturas
   promedio_humedad(cant_lec, seco);
   delay(time_mo);
   lcd.clear();

   if (prom_sh1 >= min_le && prom_sh1 <= max_le)
   {
      digitalWrite(releIN2, LOW);
      lcd.setCursor(0, 0);
      lcd.print("REGANDO LECHUGA CARRIL 1");
      lcd.setCursor(0, 1);
      lcd.print("Durante: " + String(time_le / 1000) + "s");
      delay(time_le);
      digitalWrite(releIN2, HIGH);
   }

   if (prom_sh2 >= min_le && prom_sh2 <= max_le)
   {
      digitalWrite(releIN3, LOW);
      lcd.setCursor(0, 0);
      lcd.print("REGANDO LECHUGA CARRIL 2");
      lcd.setCursor(0, 1);
      lcd.print("Durante: " + String(time_le / 1000) + "s");
      delay(time_le);
      digitalWrite(releIN3, HIGH);
   }

   int lectura = digitalRead(interruptorBombaRecirculacion);
   if (lectura == LOW)
   {
      esta_encendida = true;
   }
   else
   {
      esta_encendida = false;
   }

   if (esta_encendida)
   {
      Serial.println("encendido");
      digitalWrite(releIN1, LOW);
   }
   else
   {
      Serial.println("apagado");
      digitalWrite(releIN1, HIGH);
   }
}

void promedio_humedad(int lec, float cali_seco)
{
   float sum_sh1 = 0;
   float sum_sh2 = 0;
   for (int i = 1; i <= lec; i++)
   {
      float lec_sh1 = analogRead(pin_sh1);
      float lec_sh2 = analogRead(pin_sh2);
      float hu_sh1 = (100 - ((lec_sh1 * 100) / cali_seco)) * 2;
      float hu_sh2 = (100 - ((lec_sh2 * 100) / cali_seco)) * 2;
      sum_sh1 = sum_sh1 + hu_sh1;
      sum_sh2 = sum_sh2 + hu_sh2;
      delay(time_lectura);
   }

   prom_sh1 = sum_sh1 / lec;
   prom_sh2 = sum_sh2 / lec;
   lcd.setCursor(0, 1);
   lcd.print("s1:" + String(int(prom_sh1)) + "%");
   lcd.setCursor(7, 1);
   lcd.print("s2:" + String(int(prom_sh2)) + "%");
}