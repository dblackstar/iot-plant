#include <DHT.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define DHTPIN 4 
#define DHTTYPE DHT11 
#define BLYNK_PRINT Serial

BlynkTimer timer;
WidgetRTC rtc;
DHT dht(DHTPIN, DHTTYPE);
WidgetLCD lcd(V2);
WidgetLCD lcd2(V4);
String msg = "";

char auth[] = ""; // Your Blynk auth string here
const int sensorTierra = A0; 
const int boton = 2; 
int estado = 0;
int maximo = 1024; 
int minimo = 400; 
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
  
  rtc.begin();
}

void humedad()
{
  estado = analogRead(sensorTierra);
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
    Blynk.virtualWrite(V5, h); 
    Blynk.virtualWrite(V6, t);  
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
  Blynk.begin(auth, "your wifi router here", "your router key here");
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
