#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>

const char* SSID = "WIFI_SSID";
const char* PASS = "WIFI_PASSWORD";
const long utcOffsetInSeconds = 7200; // 2 * 60 * 60

float prevTemp = 0.0;
int prevHum = 0;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define DHTPIN 4
#define DHTTYPE DHT11
#define API_KEY "API_KEY"
#define DATABASE_URL "FIREBASE_DATABASE_URL"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

DHT dht(DHTPIN, DHTTYPE);

unsigned long sendDataPrevMilSec = 0;
unsigned long count = 0; //Change this when in production

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(SSID, PASS);
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  timeClient.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  timeClient.update();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (Firebase.ready() && (millis() - sendDataPrevMilSec > 15000 || sendDataPrevMilSec == 0))
  {
    sendDataPrevMilSec = millis();

    String tempPath = "/reader/";
    tempPath += String(count);
    tempPath += "/temperature";

    String humPath = "/reader/";
    humPath += String(count);
    humPath += "/humidity";

    String datePath = "/reader/";
    datePath += String(count);
    datePath += "/time";

    String dateVal = String(daysOfTheWeek[timeClient.getDay()]);
    dateVal += " ";
    dateVal += String(timeClient.getHours());
    dateVal += ":";
    dateVal += String(timeClient.getMinutes());
    dateVal += ":";
    dateVal += String(timeClient.getSeconds());

    // Only write to db if temperature or humidity changes
    if (t != prevTemp || h != prevHum)
    {
      Serial.printf("Set Temperature... %s\n", Firebase.setFloat(fbdo, tempPath, t) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Get Temperature... %s\n", Firebase.getFloat(fbdo, tempPath) ? String(fbdo.to<float>()).c_str() : fbdo.errorReason().c_str());
      Serial.printf("Set Humidity... %s\n", Firebase.setDouble(fbdo, humPath, h) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Get Humidity... %s\n", Firebase.getDouble(fbdo, humPath) ? String(fbdo.to<double>()).c_str() : fbdo.errorReason().c_str());
      Serial.printf("Set Current Time... %s\n", Firebase.set(fbdo, datePath, dateVal) ? "ok" : fbdo.errorReason().c_str());
      
      Serial.println();

      count++;

      prevTemp = t;
      prevHum = h;
    }

    // Get current temp & hum and set it under current
    Serial.printf("Set Current Temperature... %s\n", Firebase.setFloat(fbdo, F("current/temperature"), t) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set Current Humidity... %s\n", Firebase.setDouble(fbdo, F("current/humidity"), h) ? "ok" : fbdo.errorReason().c_str());
    
  }
}
