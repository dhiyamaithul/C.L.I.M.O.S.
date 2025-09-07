
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "ThingSpeak.h"
#include <Adafruit_BME280.h>
#include <Wire.h>

DHT dht(D7, DHT11);
Adafruit_BME280 bme;
double T, P;
char status;
WiFiClient client;

String apiKey = "your api key";
const char *ssid =  "wifi or network name";
const char *pass =  "wifi or network password";
const char* server = "api.thingspeak.com";


void setup() {
  Serial.begin(9600);
  delay(10);
  bme.begin();
  Wire.begin();
  dht.begin();
  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  //BMP280 sensor

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  

  //DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Rain sensor
  int r = analogRead(A0);
  r = map(r, 0, 1024, 0, 100);


  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(P, 2);
    postStr += "&field4=";
    postStr += String(r);
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("absolute pressure: ");
    Serial.print(P, 2);
    Serial.println("mb");
    Serial.print("Rain");
    Serial.println(r);

  }
  client.stop();
  delay(3000);
}
