/**
 * Get the temperature data from DHT22 and send the data to a webserver over wifi using Adafruit HUZZAH Feather ESP8266
 */

// DHT Sensor Program Written by ladyada, public domain
#include "DHT.h"

// Feather ESP8266 capabilities
#include <ESP8266WiFi.h>

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// wifi details
int wifiStatus = WL_IDLE_STATUS;
const char* ssid     = "some";
const char* password = "some";
WiFiClient restClient;
const char* host="192.168.1.144";
const int port=8086;
const char* writeUrl="/write?db=temperatures";

void setup() {
  
  Serial.begin(9600);

  // initialize DHT22
  dht.begin();
  Serial.println("DHT22 Initialized");

  // Connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifiConnect() {
  // initialize esp8266 wifi client
  while ( wifiStatus != WL_CONNECTED) {
    Serial.println("Attempting to connect to wifi");
    wifiStatus = WiFi.begin(ssid, password);
    // wait for 10 seconds
    delay (10000);
  }

  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //printDataToSerial(t, h, hic);
  
  // send the data to the webserver
  if (restClient.connect(host, port)) {
    Serial.println("connected.");

    // body
    String body = String("temp_celsius,room=bedroom_cottage value=") + t;

    // request url and headers
    String postRequest = String("POST ") + writeUrl + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" 
    + "Content-Type: application/x-www-form-urlencoded\r\n" 
    + "Content-Length: " + body.length() + "\r\n" 
    + "Connection: close\r\n\r\n";
    
    // send the HTTP POST request:
    restClient.print(postRequest);
    restClient.print(body);
    restClient.println();

    Serial.print(body);
    Serial.println();
  } else {
    Serial.println("Couldn't connect to the webserver");
  }
}

// print the data to serial port
void printDataToSerial(float t, float h, float hic) {
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print("%\n ");
}

