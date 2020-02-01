#include <SPI.h>
#include <LoRa.h>
#include <BluetoothSerial.h>
#include <TinyGPS++.h>
#define RXD2 16
#define TXD2 17

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

//Variables to keep track of door status
int door1 = 25, door2 = 26;
volatile int state1 =  false, state2 = false;
unsigned long previousMillis1 = 0, previousMillis2 = 0, previousMillis3 = 0, previousMillis4 = 0; 
const long interval = 1000;
int sent = 0, s1 = 0, s2 = 0;

//Defining Bluetooth object
BluetoothSerial bt;
String pwd="";

TinyGPSPlus gps;

void changeDoor1Status() {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis1 >= interval) {
        previousMillis1 = currentMillis;   
        state1 = !state1;
    }    
}

void changeDoor2Status() {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis2 >= interval) {
        previousMillis2 = currentMillis;   
        state2 = !state2;
    }    
}

void trackLocation()
{
  while(1)
  {
    while(Serial2.available()){
    gps.encode(Serial2.read());
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis4 >= 5000){
      previousMillis4 = currentMillis;
      LoRa.beginPacket();
      LoRa.print(gps.location.lat());
      LoRa.print(",");
      LoRa.print(gps.location.lng());
      LoRa.endPacket();  
   } 
  }
 }
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while (!Serial);
  Serial.println("LoRa Sender");
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(868E6)) {
    Serial.println(".");
    delay(500);
  }
  // Change sync word (0xF3) to match the receiver
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  pinMode(door1, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(door1), changeDoor1Status, CHANGE);
  pinMode(door2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(door2), changeDoor2Status, CHANGE);
  bt.begin("Car Theft Detector");
}

void loop() {
  unsigned long currentMillis = millis();
  int d1 = state1, d2 = state2;
  if(d1 && s1==0){
    s1 = 1;
    Serial.println("Door 1 OPENED");
  }
  else if(!d1 && s1==1){
    Serial.println("Door 1 CLOSED");
    s1 = 0;
  }
  if(d2 && s2==0){
    s2 = 1;
    Serial.println("Door 2 OPENED");
  }
  else if(!d2 && s2==1){
    Serial.println("Door 2 CLOSED");
    s2 = 0;
  }
  if((s1 || s2) && (currentMillis - previousMillis3 >= interval)){
    previousMillis3 = currentMillis;
    unsigned long start_time = millis();
    unsigned long current = 0;
    do
    {
       pwd=bt.readString();
       current = millis();
    }
    while(pwd=="" && current-start_time<=60000);
    Serial.println(pwd);
    if(pwd == "Hello") Serial.println("RIGHT ON BRO");
    else 
    {
      Serial.println("you suck");
      String msg;
      if(s1 && s2) msg = "Doors 1 and 2 Opened";
      else if(s1) msg = "Door 1 Opened";
      else if(s2) msg = "Door 2 Opened";
      delay(1000);
      LoRa.beginPacket();
      LoRa.print(msg);
      Serial.println(msg);
      LoRa.endPacket();
      delay(2000);
      trackLocation();
    }
  }
  delay(100);
}
