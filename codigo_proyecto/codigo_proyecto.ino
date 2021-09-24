/*
   Librerias para el uso de la pantalla LCD
   Comunica la pantalla LCD con el Arduino a travez del bus I2C
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // necesita descargarse
// Libreria para el reloj RTC
#include "RTClib.h" // necesita descargarse
/*
   Libreria para el modulo SD
   Modulo de la tarjeta SD conectada al bus SPI (formas de comunicación disponibles en Arduino) de la siguiente manera:
   MOSI (Master-out, slave-in) para la comunicación del maestro al esclavo.
   MISO (Master-in, slave-out) para comunicación del esclavo al maestro.
   SCK (Clock) señal de reloj enviada por el maestro.
   MOSI - pin 11 (uno) - pin 51 (mega)
   MISO - pin 12 (uno) - pin 50 (mega)
   CLK - pin 13 (uno) - pin 52 (mega)
   CS - pin 4
*/
#include <SPI.h>
#include <SD.h>
// Libreria para el sensor de humedad relativa y temperatura (DHT11)
// Depende de la libreria Adafruit Unified Sensor -> // necesita descargarse
#include <DHT.h> // necesita descargarse

/* 
   Variable para el manejo de la pantalla LCD
   1. direccion del dispositivo
   tamaño del LCD
   2. largo (16)
   3. ancho (2)
   Direcciones de bus del i2c
   El módulo de conversión lcd i2c cómo realiza este tipo de comunicación en el bus necesita una dirección para poder acceder a él y enviar la información
   necesaria para visualizar datos en la pantalla, como vimos anteriormente está basado en el chip PCF8574 el cual dependiendo de la versión del fabricante
   puede comenzar con una u otra dirección, esto lo podemos identificar con la siguiente tabla:
   A0	A1	A2	PCF8574	PCF8574A (modelos de modulos)
   0	0	0	0X27	   0X3F
   0	0	1	0X26	   0X3E
   0	1	0	0X25	   0X3D
   0	1	1	0X24	   0X3C
   1	0	0	0X23	   0X3B
   1	0	1	0X22	   0X3A
   1	1	0	0X21	   0X39
   1	1	1	0X20	   0X39
*/
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variable para el reloj (modelo del reloj RTC_DS3231)
RTC_DS3231 reloj;

// Variable para manipular el archivo de la tarjeta SD
File miArchivo;
byte pinCS = 53;
// Variable con el nombre del archivo en el SD
String nombreArchivo = "data.csv";

/* 
   Variables para el sensor de humedad relativa y de temperatura (SensorHRT):
   1. el pin al que se conecta
   2. el modelo de sensor (DTH11 o DTH22)
*/
byte pinSensorHRT = 2;
#define tipoSensorHRT DHT11
DHT sensorHRT(pinSensorHRT, tipoSensorHRT);

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
   Pines para los reles
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
const int mumeroLecturasParaObtenerPromedio = 10;
// tiempo entre cada una de las lecturas
const int tiempoEntreLecturasDelPromedio = 500;
// Tiempo entre cada realizacion de las lecturas por los sensores
const long tiempoEntreLecturasRealizadas = 5000;
// Tiempo entre el guardado de las lecturas realizadas por los sensores
const long tiempoEntreGuardadoDeLecturasRegistradas = 60000;

// Variables para el promedio de humedades del suelo leidas por los sensores
float promedioPorcentajesSHS1 = 0; // 0% -> 100% (usamos este)
float promedioPorcentajesSHS2 = 0; // 0% -> 100% (usamos este)

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

long tiempoMilisegundos = 0;

//Sensores de nivel de agua (flotadores)
#define pinSensorNivelInferior 3
#define pinSensorNivelSuperior 4
boolean aNivelMaximo;
boolean aNivelMinimo;

void setup()
{
   // Inicializacion puerto serial
   Serial.begin(9600);
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
   // Reloj RTC_DS3231
   if (!reloj.begin())
   {
      Serial.println("No se pudo encontrar el Reloj RTC");
      Serial.flush();
      abort();
   }
   if (reloj.lostPower())
   {
      Serial.println("¡El Reloj RTC perdió energía, ¡establezcamos la hora!");
      reloj.adjust(DateTime(F(__DATE__), F(__TIME__))); //Fecha y hora de la ultima subida de codigo (compilacion)
   }
   // Modulo tarjeta SD
   Serial.println("Inicializando tarjeta SD...");

   if (!SD.begin(pinCS))
   {
      Serial.println("¡Inicializacion fallida!");
   }
   else
   {
      Serial.println("Inicializacion completa");
   }
   // Sensor humedad relativa y temperatura
   sensorHRT.begin();
   //Sensores de nivel de agua (flotadores)
   pinMode(pinSensorNivelInferior, INPUT);
   pinMode(pinSensorNivelSuperior, INPUT);
   aNivelMinimo = false;
   aNivelMaximo = false;
}

void loop()
{
   lecturasSensores(mumeroLecturasParaObtenerPromedio, valorLecturaTierraSeca);

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

   // int lectura = digitalRead(pinInterruptor);
   // if (lectura == HIGH)
   // {
   //    encendidoBombaAguaRecirculacion = true;
   // }
   // else
   // {
   //    encendidoBombaAguaRecirculacion = false;
   // }

   byte lecturaInterruptor = digitalRead(pinInterruptor);
   if (encendidoBombaAguaRecirculacion || lecturaInterruptor == HIGH)
   {
      // Bomba de agua de recirculacion encendida
      digitalWrite(pinReleIN1, LOW);
      digitalWrite(pinLed, HIGH);
   }
   else
   {
      // Bomba de agua de recirculacion apagada
      digitalWrite(pinReleIN1, HIGH);
      digitalWrite(pinLed, LOW);
   }

   // if (encendidoBombaAguaRecirculacion)
   // {
   //    // Serial.println("Bomba de agua de recirculacion encendida");
   //    digitalWrite(pinReleIN1, LOW);
   //    digitalWrite(pinLed, HIGH);
   // }
   // else
   // {
   //    // Serial.println("Bomba de agua de recirculacion apagada");
   //    digitalWrite(pinReleIN1, HIGH);
   //    digitalWrite(pinLed, LOW);
   // }
}

void lecturasSensores(int numeroLecturas, float valorLecturaTierraSeca)
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
      // float porcentajeSHS1 = (100 - ((lecturaSHS1 * 100) / valorLecturaTierraSeca)) * 2;
      // float porcentajeSHS2 = (100 - ((lecturaSHS2 * 100) / valorLecturaTierraSeca)) * 2;
      sumaPorcentajesSHS1 = sumaPorcentajesSHS1 + porcentajeSHS1;
      sumaPorcentajesSHS2 = sumaPorcentajesSHS2 + porcentajeSHS2;
      delay(tiempoEntreLecturasDelPromedio);
   }
   promedioPorcentajesSHS1 = sumaPorcentajesSHS1 / numeroLecturas;
   promedioPorcentajesSHS2 = sumaPorcentajesSHS2 / numeroLecturas;
   DateTime ahora = reloj.now();             //obteniendo la fecha y hora "actual"
   String marcaDeTiempo = ahora.timestamp(); // obtener la marca de tiempo "actual"
   float humedadRelativa = sensorHRT.readHumidity();
   float temperaturaCelcius = sensorHRT.readTemperature();
   boolean lecturasSensorHRTSonValidas = !isnan(humedadRelativa) && !isnan(temperaturaCelcius); // isnan() -->  is not a number, verifica si el valor no es un numero (nan -> not a number)
   // Guardar en tarjeta SD
   if (millis() - tiempoMilisegundos >= tiempoEntreGuardadoDeLecturasRegistradas || tiempoMilisegundos == 0)
   {
      tiempoMilisegundos = millis();
      miArchivo = SD.open(nombreArchivo, FILE_WRITE); // abriendo el archivo para escritura
      if (miArchivo)
      {
         if (lecturasSensorHRTSonValidas)
         {
            String registro = String(promedioPorcentajesSHS1) + "," + String(promedioPorcentajesSHS2) + "," + String(humedadRelativa) + "," + String(temperaturaCelcius) + "," + marcaDeTiempo; // concatenamos las lecturas separandolos con comas (formato posible para un .csv)
            miArchivo.println(registro);
            Serial.println(registro);
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print("GUARDANDO");
            lcd.setCursor(3, 1);
            lcd.print("REGISTRO...");
            delay(1000);
         }
         else
         {
            Serial.println("¡Lecturas no validas!");
         }
         miArchivo.close();
      }
      else
      {
         Serial.println("¡Error abriendo " + nombreArchivo + "!");
      }
   }
   // Mostrar lecturas en LCD
   lcd.clear();
   lcd.setCursor(0, 0);
   if (lecturasSensorHRTSonValidas)
   {
      lcd.print("HR:" + String(int(humedadRelativa)) + "     " + String(int(temperaturaCelcius)) + "C");
   }
   else
   {
      lcd.print("HUMEDAD SUELO");
   }
   lcd.setCursor(0, 1);
   lcd.print("S1:" + String(int(promedioPorcentajesSHS1)) + "%");
   lcd.setCursor(9, 1);
   lcd.print("S2:" + String(int(promedioPorcentajesSHS2)) + "%");
   delay(tiempoEntreLecturasRealizadas);
   lcd.clear();
}
