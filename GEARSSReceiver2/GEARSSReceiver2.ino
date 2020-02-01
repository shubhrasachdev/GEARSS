#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

//Credentials
const char* ssid = "Shubhra";
const char* pwd = "SAVEBANDIT16";
char auth[] = "3PaGfxfl9d84XBHmL-ShX4RA2khFJjMq";

//Blynk Map Widget
WidgetMap myMap(V2);

//Blynk Terminal Widget
WidgetTerminal terminal(V3);

//HTTP object
HTTPClient http;

//Buzzer Pin
int pin = 25;
int pin_status = 0;

//Previous received message
String prevLora = "", currentLora = "";

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 0) 
  {
    digitalWrite(pin, LOW);
    pin_status = 0;
  }
}

BLYNK_WRITE(V3)
{
  if(prevLora != currentLora)
  {
    prevLora = "";
    terminal.println("Unnamed Road, Rohini Sector 13, New Delhi, Delhi. 215 m from Badli Water Plant pin-110085 (India)");
  }
}

void trackLocation(){
  while(1)
  {
    int packetSize = LoRa.parsePacket();
    if (packetSize) 
    {
      // received a packet
      Serial.print("Received packet: ");
      while (LoRa.available()) {
        currentLora = LoRa.readString();
        Serial.println(currentLora);
        int split = currentLora.indexOf(",");
        String latitude = currentLora.substring(0, split);
        String longitude = currentLora.substring(split+1);
        float la = latitude.toFloat();
        float ln = longitude.toFloat();
        int index = 0;
        myMap.location(index, la, ln, "value");
        Blynk.run();
        prevLora = currentLora;  
        /*http.begin("http://connect123.pythonanywhere.com/postjson");
        //http.begin("http://jsonplaceholder.typicode.com/posts");  //Specify destination for HTTP request
        http.addHeader("Content-Type", "application/json");
        String json = "{\"Latitude\":"+ latitude+ ", \"Longitude\":"+ longitude+ "}";
        int httpResponseCode = http.POST(json);   //Send the actual POST request
        if(httpResponseCode>0){
          String response = http.getString();                       //Get the response to the request
          Serial.println(httpResponseCode);   //Print return code
          Serial.println(response);           //Print request answer
        }
        else{
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
        }
        http.end();  //Free resources*/
      }
    }
    Blynk.run();
  } 
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");
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
  WiFi.begin(ssid, pwd);
  while(WiFi.status()!=WL_CONNECTED) 
  {
    Serial.println(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi");
  pinMode(pin, OUTPUT);
  Blynk.begin(auth, ssid, pwd);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet: ");
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      Blynk.notify(LoRaData);
      digitalWrite(pin, HIGH);
      pin_status = 1;
    }
  }
  else pin_status = 0;
  Blynk.virtualWrite(V1, pin_status);
  Blynk.run();
  if (pin_status == 1) trackLocation();
}
