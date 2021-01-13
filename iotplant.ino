#include <DHT.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define DHTPIN 4 // PIN Digital 2 conectado a la libreria del sensor
#define DHTTYPE DHT11 // El tipo de sensor que estoy usando es el DHT11
#define BLYNK_PRINT Serial

BlynkTimer timer;
WidgetRTC rtc;
DHT dht(DHTPIN, DHTTYPE);
WidgetLCD lcd(V2);
WidgetLCD lcd2(V4);
String msg = "";

char auth[] = "Dz1hj3XaFSdLU4ExpMMPMjSjNvdeatrh";
const int sensorTierra = A0; // PIN Analogo 0
const int boton = 2; // PIN Digital 4
int estado = 0;
int maximo = 1024; // El valor del sensor en tierra completamente seca
int minimo = 400; // El valor del sensor en agua
int estadoBoton = 0;
String hora = "";
String fecha = "";

String reloj() 
{
    return hora = String(hour()) + ":" + minute() + ":" + second();
    return fecha = String(day()) + "/" + month() + "/" + year();
}

void grabarHora()
{
  reloj();
  int valor = digitalRead(boton);
  
  if (estadoBoton != valor) 
  {
    estadoBoton = valor;
    Blynk.virtualWrite(V3, fecha);
    Blynk.virtualWrite(V4, hora);
    Serial.print(hora);
    Serial.print(" ");
    Serial.print(fecha);
    Serial.println();
  }  

}

BLYNK_CONNECTED() 
{
  // Sincronizar la hora al momento de conectarse
  rtc.begin();
}

void humedad()
{
  estado = analogRead(sensorTierra);
  // cuando mapeamos los valores a un porcentaje, los primeros dos parametros representan
  // el valor maximo del sensor en tierra seca, y el segundo su valor minimo en tierra humeda.
  // estos valores se necesitan ajustar dependiendo el tipo de tierra, el sensor, la planta, y de en donde se encuentre la planta
  estado = map(estado,maximo,minimo,0,100); 
  Serial.print("Humedad en la tierra: "); 
  Serial.print(estado);
  Serial.println("%");

  if (estado < 15) 
  {
    Serial.println("La planta necesita agua, mandar notificación");
    Blynk.notify("La planta necesita agua");
  } 
 
  else 
  {
    Serial.println("La planta no necesita agua");
    delay(1000);
  }   
  
  msg = msg + estado;  
  lcd.print(0,0," Humedad en la");
  lcd.print(0,1," tierra: "+msg+"%");
  msg = "";
}

void temperatura() 
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Blynk.virtualWrite(V5, h);  // PIN virtual 5 para mostrar humedad
    Blynk.virtualWrite(V6, t);  // PIN virtual 5 para mostrar temperatura
    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C ");  
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, "MEGACABLE-2E9E", "BKwg4rqu");
  pinMode(sensorTierra, INPUT);
  pinMode(boton, INPUT_PULLUP);
  dht.begin();
  setSyncInterval(10 * 60); 
  timer.setInterval(1000L, temperatura);
  timer.setInterval(1000L, humedad);
  timer.setInterval(1000L, grabarHora);
}

void loop()
{ 
  Blynk.run();
  timer.run();
}
