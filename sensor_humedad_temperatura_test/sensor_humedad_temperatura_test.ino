#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
  Serial.begin(9600);

  dht.begin();
  
}

void loop() {
  
  delay(5000);
 
  float humedadRelativa = dht.readHumidity();
  float temperaturaCelcius = dht.readTemperature();
  float temperaturaFarentheit = dht.readTemperature(true);
 
  if (
      isnan(humedadRelativa) ||
      isnan(temperaturaCelcius) ||
      isnan(temperaturaFarentheit)
      ) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }

  float indiceCalorCelcius = dht.computeHeatIndex(temperaturaCelcius, humedadRelativa, false);
  float indiceCalorFarenheit = dht.computeHeatIndex(temperaturaFarentheit, humedadRelativa);
 
  Serial.print("Humedad relativa: ");
  Serial.print(humedadRelativa);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperaturaCelcius);
  Serial.print(" °C ");
  Serial.print(temperaturaFarentheit);
  Serial.print(" °F\t");
  Serial.print("Índice de calor: ");
  Serial.print(indiceCalorCelcius);
  Serial.print(" °C ");
  Serial.print(indiceCalorFarenheit);
  Serial.println(" °F");
 
}
