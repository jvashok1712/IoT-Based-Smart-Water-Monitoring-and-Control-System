#include <SPI.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

// Define pins and credentials
SoftwareSerial myserial(D1, D2);
#define VALVE D0

#define FIREBASE_HOST "https://level-monitoring-system-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "KdjXtHpBMfENfUIw81nXAeRYQ7bVV8AVw7EsQ767"

#define WIFI_SSID "ashok"
#define WIFI_PASSWORD "ashoku1712"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  myserial.begin(9600);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to WiFi.");

  // Initialize Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, NULL);
  Firebase.reconnectWiFi(true);

  // Set up the valve pin
  pinMode(VALVE, OUTPUT);
  digitalWrite(VALVE, HIGH); // Default: valve OFF
}

void loop() {
  String msg = myserial.readStringUntil('\r');
  Serial.println(msg);

  // Parsing incoming data (example format)
  int l, ph, tds, cost, dis, fl;
  float lit, co, flph;
  String s1, s2, s3, s4, s5;
  sscanf(msg.c_str(), "%s %d %s %d %s %d %s %d %s %d", s1, &l, s2, &dis, s3, &tds, s4, &cost, s5, &fl);
  lit = l / 100.0;
  co = lit * 10.0;
  flph = fl / 100.0;

  Serial.println("Parsed values:");
  Serial.println("Liters: " + String(lit));
  Serial.println("TDS: " + String(tds));
  Serial.println("Distance: " + String(dis));

  // Retrieve valve status from Firebase
  bool valveStatus;
  if (Firebase.getBool(firebaseData, "/main/valve", &valveStatus)) {
    Serial.print("Valve status retrieved: ");
    Serial.println(valveStatus ? "ON" : "OFF");
    digitalWrite(VALVE, valveStatus ? LOW : HIGH); // LOW = ON, HIGH = OFF
  } else {
    Serial.println("Failed to retrieve valve status: " + firebaseData.errorReason());
  }

  // Send sensor data to Firebase
  if (Firebase.setInt(firebaseData, "/main/sensor1/value", dis)) {
    Serial.println("Ultrasonic data sent successfully.");
  } else {
    Serial.println("Failed to send ultrasonic data: " + firebaseData.errorReason());
  }
  Firebase.setString(firebaseData, "/main/sensor1/name", "Ultrasonic Sensor");
  Firebase.setString(firebaseData, "/main/sensor1/msg", "Depth");
  Firebase.setString(firebaseData, "/main/sensor1/units", "cm");

  if (Firebase.setInt(firebaseData, "/main/sensor2/value", lit)) {
    Serial.println("Water flow data sent successfully.");
  } else {
    Serial.println("Failed to send water flow data: " + firebaseData.errorReason());
  }
  Firebase.setString(firebaseData, "/main/sensor2/name", "Water Flow Sensor");
  Firebase.setString(firebaseData, "/main/sensor2/msg", "Waterflow");
  Firebase.setString(firebaseData, "/main/sensor2/units", "L");

  if (Firebase.setInt(firebaseData, "/main/sensor3/value", tds)) {
    Serial.println("TDS data sent successfully.");
  } else {
    Serial.println("Failed to send TDS data: " + firebaseData.errorReason());
  }
  Firebase.setString(firebaseData, "/main/sensor3/name", "TDS Sensor");
  Firebase.setString(firebaseData, "/main/sensor3/msg", "Purity");
  Firebase.setString(firebaseData, "/main/sensor3/units", "ppm");

  if (Firebase.setInt(firebaseData, "/main/sensor4/value", flph)) {
    Serial.println("Flow rate data sent successfully.");
  } else {
    Serial.println("Failed to send flow rate data: " + firebaseData.errorReason());
  }
  Firebase.setString(firebaseData, "/main/sensor4/name", "Flow Sensor");
  Firebase.setString(firebaseData, "/main/sensor4/msg", "Flow rate");
  Firebase.setString(firebaseData, "/main/sensor4/units", "L/hr");

  // Delay for stability
  delay(1000);
}
