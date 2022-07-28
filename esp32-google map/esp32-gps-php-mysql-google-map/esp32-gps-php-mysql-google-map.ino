#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <WiFi.h>
#include <HTTPClient.h>

//enter WIFI credentials
const char* ssid     = "Wi-Fi SSID";
const char* password = "Password";

const char* SERVER_NAME = "https://gpstracing.000webhostapp.com/gpsdata.php";

//ESP32_API_KEY is the exact duplicate of, ESP32_API_KEY in config.php file
//Both values must be same
String ESP32_API_KEY = "Ad5F10jkBM0";

//Send an HTTP POST request every 30 seconds
unsigned long previousMillis = 0;
long interval = 30000;

//GSM Module RX pin to ESP32 2
//GSM Module TX pin to ESP32 4
#define rxPin 4
#define txPin 2
HardwareSerial neogps(1);

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  Serial.println("esp32 serial initialize");

  neogps.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("neogps serial initialize");
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
       previousMillis = currentMillis;
       sendGpsToServer();
    }
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(1000);  
}

int sendGpsToServer()
{
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while(neogps.available()){
      if(gps.encode(neogps.read())){
        if(gps.location.isValid() == 1){
          newData = true;
          break;
        }
      }
    }
  }

  if(true){
    newData = false;
  
    String latitude, longitude;
    
    latitude = String(gps.location.lat(), 6); // Latitude in degrees (double)
    longitude = String(gps.location.lng(), 6); // Longitude in degrees (double)
    
    //altitude = gps.altitude.meters(); // Altitude in meters (double)
    //date = gps.date.value(); // Raw date in DDMMYY format (u32)
    //time = gps.time.value(); // Raw time in HHMMSSCC format (u32)
    //speed = gps.speed.kmph();
    
    //Serial.print("Latitude= "); 
    //Serial.print(latitude);
    //Serial.print(" Longitude= "); 
    //Serial.println(longitude);
    HTTPClient http;
    
    http.begin(SERVER_NAME);
    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String gps_data;
    gps_data = "api_key="+ESP32_API_KEY;
    gps_data += "&lat="+latitude;
    gps_data += "&lng="+longitude;

    Serial.print("gps_data: ");
    Serial.println(gps_data);
    int httpResponseCode = http.POST(gps_data);
    String httpResponseString = http.getString();
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(httpResponseString);
    }
    else {
      Serial.print("Error on HTTP request - Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(httpResponseString);
    }
    http.end();
  }  
}
