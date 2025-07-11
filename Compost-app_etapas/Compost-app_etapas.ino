#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_Sensor.h>


#include <Arduino.h>
#include <MHZ19.h>
#include <SoftwareSerial.h>


#define sensorPH 36
float lecturaSensorPh;
float tension;
float calibracion_ph(float medidaPh);
float phCalibrado;
float valorPhReal;


SoftwareSerial co2Serial(16,17 ); // RX, TX (Arduino recibe en 10, transmite en 11)
MHZ19 myMHZ19;
float ppmReal;



#define SENSORMQ4R 39
float lecturaMetanoR;

#define SENSORMQ4L 39
float lecturaMetanoL;

#define FC28_R 35
#define FC28_L 32
int lecturaFc28R;
int lecturaFc28L;
float promedioHumedad();
float lecturaTotal;
float promedio;
float promedioReal;
float porcentajeHumedad;
unsigned long tiempoAhora;
unsigned long tiempoUltimoCambio;
unsigned long intervaloHumedad = 60000;
unsigned long intervaloPromedio = 900000;

#define DHTTYPE DHT22
#define DHTPIN 34
DHT dht(DHTPIN, DHTTYPE);
float temperatura;

void Etapas(float lec1, float lec2, float lec3, float lec4);
String etapa;
void emergencias ();

void setup() {
  Serial.begin(9600);
  co2Serial.begin(9600);                               // (Uno example) device to MH-Z19 serial start
  myMHZ19.begin(co2Serial);                                // *Serial(Stream) refence must be passed to library begin().
  myMHZ19.autoCalibration();

}

void loop() {
  lecturaSensorPh = analogRead(sensorPH);

  valorPhReal = calibracion_ph(lecturaSensorPh);
  ppmReal = myMHZ19.getCO2();
  lecturaMetanoR = analogRead(SENSORMQ4R);
  lecturaMetanoL = analogRead(SENSORMQ4L);

  lecturaFc28R = analogRead(FC28_R);
  lecturaFc28L = analogRead(FC28_L);
  temperatura = analogRead(DHTPIN);
  //Serial.println(valorPhReal);
  promedioReal = promedioHumedad();
  porcentajeHumedad = map(promedioReal,0,4095,0,100);
  Serial.println(ppmReal);
  Etapas(temperatura,porcentajeHumedad, valorPhReal, ppmReal);
  Serial.println(etapa);
}

float calibracion_ph(float medidaPh){
  tension = medidaPh * 5/ 1024;
  phCalibrado = 7+((2.5 - tension)/0.1841);

  return phCalibrado;
}

void Etapas(float lec1, float lec2, float lec3, float lec4){
  
  if( lec1 >= 45 && lec1 <= 60 && lec2 >= 50 && lec2 <= 60 && lec3 >= 6.5){
    etapa = "Etapa mesofilica";
  }
  else if( lec1 <= 45 && lec1 >= 20 && lec2 >= 45 && lec2 <= 55 && lec3 >= 6 && lec4 >= 800){
    etapa = "Etapa termofilica";
  }
  else if (lec1 >= 15 && lec1 >= 25 && lec2 >= 30 && lec2 <= 40 && lec3 >= 7){
    etapa = "Etapa Maduración";
  }
}

void emergencias(){
  
}

float promedioHumedad(){
  tiempoAhora = millis();

  if(tiempoAhora - tiempoUltimoCambio >= intervaloHumedad){
    tiempoUltimoCambio = tiempoAhora;
    lecturaTotal = lecturaFc28R + lecturaFc28L;
  }
  if(tiempoAhora - tiempoUltimoCambio >= intervaloPromedio){
    tiempoUltimoCambio = tiempoAhora;
    promedio = lecturaTotal / 15;
  }
  return promedio
}
