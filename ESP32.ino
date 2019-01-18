#include "IOXhop_FirebaseStream.h"
//http://forum.arduino.cc/index.php/topic,106043.0.html
//pin sensor 1
#define trigPin 2
#define echoPin 5

//pin sensor 2
#define trigPin2 18
#define echoPin2 19

//verifica si esta pasando
String sensor1;
String sensor2;

//para ver si hay persona en el rango
long distance1;
long distance2;

long duration1;
long duration2;

int minRange = 25;

//sensor primero en pasar persona
String primero;
String segundo;

//cantidad de personas
int nPersonas;
String sPersonas;

//LED
const int ledPin = 22;
const int ledPin2 = 23;

//librerias de wifi
#include <WiFi.h>
//firebase
#include <IOXhop_FirebaseESP32.h>

//credenciales firebase y wifi
#define FIREBASE_HOST "personcounter-b0162.firebaseio.com"
#define FIREBASE_AUTH "yrhrjC2XESm3r1YFhAnFJPAzctk2mUhaQCNn5gTD"
#define WIFI_SSID "Carlos"
#define WIFI_PASSWORD "Carlos123"
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  //sensor 1
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  // connect to firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //LED
  pinMode (ledPin, OUTPUT);
  pinMode (ledPin2, OUTPUT);

  //inizializar estado del sensor
  sensor1 = "inactivo";
  sensor2 = "inactivo";
}

void loop() 
{
  Serial.println(distance2);
  Serial.println(distance1);
  // put your main code here, to run repeatedly:
  /* The following trigPin/echoPin cycle is used to determine the
  distance of the nearest object by bouncing soundwaves off of it. */   
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, LOW);
  duration1 = pulseIn(echoPin, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  distance1 = duration1*0.034/2;

  //segundo sensor
  digitalWrite(trigPin2, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  distance2 = duration2*0.034/2;
 
  //Serial.print("Distance1: ");
  //Serial.println(distance1);
  //Serial.print("Distance2: ");
  //Serial.println(distance2);
  
  if (distance1 <= minRange)
  {
    sensor1 = "pasando";
    digitalWrite (ledPin, HIGH);  // turn on the LED
    //pasando
    if(primero == "")
    { 
      primero = "sensor1";
    }
    else if (primero == "sensor2")
    {
      segundo = "sensor1";  
    }
  }
  else
  {
    //no hay nadie  en el rango
    sensor1 = "inactivo";
    digitalWrite (ledPin, LOW);  // turn on the LED
  }

  //verifico si la distancia es menor a 100 - significa que hay persona en el rango del sensor dos
  if (distance2 <= minRange)
  {
    sensor2 = "pasando";
    digitalWrite (ledPin2, HIGH);  // turn on the LED
    //pasando
    if(primero == "")
    { 
      primero = "sensor2";
    }
    else if (primero == "sensor1")
    {
      segundo = "sensor2";  
    }
  }
  else
  {
    //no hay nadie  en el rango
    sensor2 = "inactivo";
    digitalWrite (ledPin2, LOW);  // turn on the LED
  }
  //Serial.println("primero " + primero);
  //Serial.println("segundo " + segundo);
  //Serial.println("sensor1 " + sensor1);
  //Serial.println("sensor2 " + sensor2);
  //Serial.println(distance2);
  //Serial.println(distance1);
  
  //verifico si la persona ya no esta en rango con ningun sensor y hago el conteo
  int i,len;
  
  if (sensor2 == "inactivo")
  {
    if (sensor1 == "inactivo")
    { 
        //si huvo alguien que paso por los sensores
        if(primero == "sensor1")
        {
          if(segundo == "sensor2")
          { 
              //direccion entrando              
              //obtengo numero de personas de base de datos
              sPersonas = Firebase.getString("Actuales");
              //convierto string a numero
              nPersonas = 0;
              len = sPersonas.length();
              for(i=0; i<len; i++)
                {
                  nPersonas = nPersonas * 10 + ( sPersonas[i] - '0' );
                }
              //sumo
              //Serial.println("sumo");
              //Serial.println(distance2);
              //Serial.println(distance1);
              //Serial.println(nPersonas);
              nPersonas = nPersonas + 1;
              //Serial.println(nPersonas);
              //paso a string
              sPersonas = String(nPersonas);
              //guardo
              Firebase.setString("Actuales", sPersonas);
              primero = "";
              segundo = "";
              //Serial.println("Numero de personas: " + personas );        
          }
        }
        else if(primero == "sensor2")
        {
          if(segundo == "sensor1")
          {
              sPersonas = Firebase.getString("Actuales");
              nPersonas = 0;
              len = sPersonas.length();
              for(i=0; i<len; i++)
                {
                  nPersonas = nPersonas * 10 + ( sPersonas[i] - '0' );
                }
              if (nPersonas > 0)
              {
                //resto
                //Serial.println("resto");
                //Serial.println(distance2);
                //Serial.println(distance1);
                nPersonas = nPersonas - 1;
                //paso a string
                sPersonas = String(nPersonas);
                //guardo
                Firebase.setString("Actuales", sPersonas);
                primero = "";
                segundo = "";
              }
               
          } 
        }
    }
  }
  
}
