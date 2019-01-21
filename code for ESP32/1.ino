#include "IOXhop_FirebaseStream.h"
//pin sensor 1
#define trigPin 2
#define echoPin 5

//pin sensor 2
#define trigPin2 18
#define echoPin2 19

//verify is a person is on the sensor range
String sensor1;
String sensor2;

//distance in cm
long distance1;
long distance2;

//duration of the sensor waves
long duration1;
long duration2;

//range of the sensor to count
int minRange = 25;

//define the first and second sensor in order to make the addition or subtraction
//primero = first in spanish
String primero;
//segundo = second in spanish
String segundo;

//number of persons
int nPersonas;

//data from firebase
String sPersonas;

//LED
const int ledPin = 22;
const int ledPin2 = 23;

//Wifi librery
#include <WiFi.h>
//firebase librery
#include <IOXhop_FirebaseESP32.h>

//credential for firebase and wifi
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  //sensor 1
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //sensor 2
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

  //detefine state
  //inactivo = inactive in spanish
  //pasando = moving in spanisg
  sensor1 = "inactivo";
  sensor2 = "inactivo";
}

void loop() 
{
  // put your main code here, to run repeatedly:
  /* The following trigPin/echoPin cycle is used to determine the
  distance of the nearest object by bouncing soundwaves off of it. */   
  //first sensor
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin, LOW);
  duration1 = pulseIn(echoPin, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  distance1 = duration1*0.034/2;

  //second sensor
  digitalWrite(trigPin2, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  distance2 = duration2*0.034/2;

  //person is on range with sensor 1
  if (distance1 <= minRange)
  {
    //change state to "moving", and define the if it the first or second sensor the person is passing by
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
    //no one is on the sensor 1 range
    sensor1 = "inactivo";
    digitalWrite (ledPin, LOW);  // turn on the LED
  }

  //person is on the range of sensor 2
  if (distance2 <= minRange)
  {
     //change state to "moving", and define the if it the first or second sensor the person is passing by
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
    //no one is on the sensor 2 range
    sensor2 = "inactivo";
    digitalWrite (ledPin2, LOW);  // turn on the LED
  }
  
  
  int i,len;
  //verify is no one is moving on the sensor range
  if (sensor2 == "inactivo")
  {
    if (sensor1 == "inactivo")
    { 
        //verify is the fist and second sensor are define
        if(primero == "sensor1")
        {
          if(segundo == "sensor2")
          { 
              //this direction is addition              
              //get number of person from database
              sPersonas = Firebase.getString("Actuales");
              //convert string to number
              nPersonas = 0;
              len = sPersonas.length();
              for(i=0; i<len; i++)
                {
                  nPersonas = nPersonas * 10 + ( sPersonas[i] - '0' );
                }
              nPersonas = nPersonas + 1;
              sPersonas = String(nPersonas);
              //save on firebase
              Firebase.setString("Actuales", sPersonas);
              //start over the sensor direction
              primero = "";
              segundo = "";
                   
          }
        }
        //the other direction, person is comming out
        else if(primero == "sensor2")
        {
          if(segundo == "sensor1")
          {
              //get number of person from database
              sPersonas = Firebase.getString("Actuales");
               //convert string to number
              nPersonas = 0;
              len = sPersonas.length();
              for(i=0; i<len; i++)
                {
                  nPersonas = nPersonas * 10 + ( sPersonas[i] - '0' );
                }
                
              if (nPersonas > 0)
              {
                nPersonas = nPersonas - 1;
                sPersonas = String(nPersonas);
                //save on database
                Firebase.setString("Actuales", sPersonas);
                //start over the sensor direction
                primero = "";
                segundo = "";
              }
               
          } 
        }
    }
  }
  
}
