// Simple sketch for sending data to the TMEP.cz
// by mikrom (http://www.mikrom.cz)
// http://wiki.tmep.cz/doku.php?id=zarizeni:esp8266
//
// If you send only temperature url will be: http://ahoj.tmep.cz/?mojemereni=25.6 (domain is "ahoj" and guid is "mojemereni"
// If you send also humidity url will be: http://ahoj.tmep.cz/?mojemereni=25.6&humV=60
// but for humidity You will need DHT11 or DHT22 sensor and code will need some modifications
//
// Not tested, but should work! :)

#include <ESP8266WiFi.h> // WiFi library
#include "DHT.h"         // DHT sensor library

// Define settings
const char* ssid    = "Belesovi"; // WiFi SSID
const char* pass    = "12345678"; // WiFi password
const char* domain  = "pocasi";    // domain.tmep.cz
const char* guid    = "temp";      // mojemereni
//const long sleep    = 600000;            // How often send data to the server. Default is 600000ms = 10 minute
const long sleep    = 60000;
const byte dhtPin   = 5;                 // Pin where is DHT connected
const byte dhtType  = 22;                // DHT type, for DHT11 = 11, for DHT22 = 22
int dht_vcc = 4; //D2
int dht_gnd = 0; //D3

DHT dht(dhtPin, dhtType); // Initialize DHT sensor.

void calculate(){

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

  //float h = random(70,90);
  //float t = random (10,15);
  
  // Connect to the HOST and send data via GET method
  WiFiClient client; // Use WiFiClient class to create TCP connections
  
  char host[50];            // Joining two chars is little bit difficult. Make new array, 50 bytes long
  strcpy(host, domain);     // Copy /domain/ in to the /host/
  strcat(host, ".beles.cz"); // Add ".tmep.cz" at the end of the /host/. /host/ is now "/domain/.tmep.cz"
  const int httpPort = 80;
  
  Serial.print("Connecting to "); Serial.println(host);
  if (!client.connect(host, httpPort)) {
    // If you didn't get a connection to the server
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Client connected");

  // Make an url. We need: /?guid=t&humV=h
  String url = "/?";
         url += guid;
         url += "=";
         url += t;
         url += "&humV=";
         url += h;
  Serial.print("Requesting URL: "); Serial.println(url);
  
  // Make a HTTP GETrequest.
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  // Workaroud for timeout
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  Serial.println();
  
  // Wait for another round
 // delay(sleep);
}

void setup() {
  // Start serial
  Serial.begin(115200);
  delay(10);
  Serial.println();
   pinMode(dht_vcc, OUTPUT);
  pinMode(dht_gnd, OUTPUT);
  
  digitalWrite(dht_vcc, HIGH);
  digitalWrite(dht_gnd, LOW);
  
  // Connect to the WiFi
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Serial.println();

 // dht.begin(); // Initialize the DHT sensor
  delay(3000);

  calculate();

  digitalWrite(dht_vcc, LOW);
  digitalWrite(dht_gnd, LOW);
  pinMode(dht_vcc, INPUT);
  pinMode(dht_gnd, INPUT);
  Serial.println("Going into deep sleep for 15 minutes");
  ESP.deepSleep(9e8);
}

void loop() {

}
