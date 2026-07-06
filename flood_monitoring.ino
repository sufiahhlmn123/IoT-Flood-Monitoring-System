/*
 * ============================================================
 *  IoT-Based Flood Monitoring System
 *  Board : NodeMCU ESP8266 (ESP-12E)
 *  Sensor: Ultrasonic HC-SR04
 *  Cloud : ThingSpeak
 *  Author: Aisyah Aina Sufia binti Hilman
 * ------------------------------------------------------------
 *  Measures water level with an ultrasonic sensor, gives local
 *  LED + buzzer alerts, and streams the reading to ThingSpeak
 *  over Wi-Fi every 10 seconds.
 *
 *  NOTE: Replace the placeholder credentials below with your
 *  own before uploading. Do NOT commit real Wi-Fi passwords or
 *  API keys to a public repository.
 * ============================================================
 */

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

// ---------- Pin definitions ----------
const int trigPin1 = D1;      // Ultrasonic Trig
const int echoPin1 = D2;      // Ultrasonic Echo
#define redled D3             // Red LED  (Danger)
#define grnled D4             // Green LED (Safe)
#define BUZZER D5             // Passive buzzer

// ---------- ThingSpeak & Wi-Fi credentials ----------
unsigned long ch_no      = 1772692;                  // Your ThingSpeak Channel ID
const char *  write_api  = "YOUR_WRITE_API_KEY";     // Your ThingSpeak Write API Key
char auth[]              = "YOUR_CHANNEL_AUTHOR_KEY"; // Your ThingSpeak Author Key
char ssid[]             = "YOUR_WIFI_SSID";           // Your Wi-Fi network name
char pass[]             = "YOUR_WIFI_PASSWORD";       // Your Wi-Fi password

// ---------- Timing ----------
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;   // upload interval (ms) = 10 seconds

WiFiClient client;
long duration1;
int  distance1;

void setup()
{
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(redled, OUTPUT);
  pinMode(grnled, OUTPUT);
  digitalWrite(redled, LOW);
  digitalWrite(grnled, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
  startMillis = millis();   // initial start time
}

void loop()
{
  // ---------- Read distance from HC-SR04 ----------
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;   // distance in centimetres
  Serial.println(distance1);

  // ---------- Alert logic ----------
  if (distance1 <= 4)          // DANGER: water is close to the sensor
  {
    digitalWrite(D3, HIGH);    // red LED ON
    tone(BUZZER, 300);         // buzzer ON (300 Hz)
    digitalWrite(D4, LOW);     // green LED OFF
    delay(500);
    noTone(BUZZER);
  }
  else                         // SAFE
  {
    digitalWrite(D4, HIGH);    // green LED ON
    digitalWrite(D3, LOW);     // red LED OFF
  }

  // ---------- Upload to ThingSpeak every 10 s ----------
  currentMillis = millis();
  if (currentMillis - startMillis >= period)
  {
    ThingSpeak.setField(1, distance1);
    ThingSpeak.writeFields(ch_no, write_api);
    startMillis = currentMillis;
  }
}
