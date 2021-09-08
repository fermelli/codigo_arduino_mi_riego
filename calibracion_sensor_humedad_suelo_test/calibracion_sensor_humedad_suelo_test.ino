/* 
   Pines para los sensores de humedad
   Los pines para los sensores de humedad de suelo deben ser analógicos
   SH1 -> Sensor de humedad 1
   SH2 -> Sensor de humedad 2
*/
const float pinSH1 = A0;
const float pinSH2 = A1;

/*
   La placa Arduino detectará el nivel de voltaje entregado por el sensor de humedad de suelo, y lo convertirá en un número equivalente a la cantidad de humedad detectada entre 0 y 1023
*/

//Numero de lecturas del sensor de humedad para el promedio
const byte cantidadLecturas = 10;
//tiempo entre cada uno de las lecturas
const byte tiempoEntreCadaLectura = 500;
//tiempo para mostrar los resultados de la lectura
const int tiempoResultados = 2000;

//Variables del promedio de lectura de humedades
float promedioPorcentajeSH1 = 0;
float promedioPorcentajeSH2 = 0;

long cantidadEjecuciones;

void setup()
{
   cantidadEjecuciones = 0;
   Serial.begin(9600);
}

void loop()
{
   //Se llama a procedimiento que promedia las lecturas
   promedio_humedad(cantidadLecturas);
   delay(tiempoResultados);
}

void promedio_humedad(byte cantidad)
{
   float sumaPorcentajeSH1 = 0;
   float sumaPorcentajeSH2 = 0;
   float sumaLecturasSH1 = 0;
   float sumaLecturasSH2 = 0;
   for (int i = 1; i <= cantidad; i++)
   {
      float lecturaSH1 = analogRead(pinSH1);
      float lecturaSH2 = analogRead(pinSH2);
      sumaLecturasSH1 = sumaLecturasSH1 + lecturaSH1;
      sumaLecturasSH2 = sumaLecturasSH2 + lecturaSH2;
      float porcentajeSH1 = map(lecturaSH1, 1021, 305, 0, 100);
      float porcentajeSH2 = map(lecturaSH2, 1022, 275, 0, 100);
      sumaPorcentajeSH1 = sumaPorcentajeSH1 + porcentajeSH1;
      sumaPorcentajeSH2 = sumaPorcentajeSH2 + porcentajeSH2;
      delay(tiempoEntreCadaLectura);
   }

   promedioPorcentajeSH1 = sumaPorcentajeSH1 / cantidad;
   promedioPorcentajeSH2 = sumaPorcentajeSH2 / cantidad;
   cantidadEjecuciones = cantidadEjecuciones + 1;
   Serial.println("------------------" + String(cantidadEjecuciones) + "----------------");
   Serial.println("s1: " + String(sumaLecturasSH1 / cantidad));
   Serial.println("s1:" + String(int(promedioPorcentajeSH1)) + "%");
   Serial.println("s2: " + String(sumaLecturasSH2 / cantidad));
   Serial.println("s2:" + String(int(promedioPorcentajeSH2)) + "%");
}
