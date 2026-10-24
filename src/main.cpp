#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define ledPin 6
#define pirPin 5

const char* ssid = "Airtel_My_Network";
const char* password = "9910357357";

const char* serverUrl = "http://httpbin.org/post";

volatile bool motionDetected = false;

void IRAM_ATTR handleMotion() {
  motionDetected = true;
}

void sendPostRequest() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // Valid C++ string escapes and valid JSON structure
  String payload = "{\"device\":\"YD-ESP32-S3\",\"ledPin\":4,\"event\":\"motion detected\"}";

  Serial.println("Sending post request with payload: " + payload);
  int httpresponse = http.POST(payload);

  if (httpresponse > 0) {
    String response = http.getString();
    Serial.println("Httpresponse code: " + String(httpresponse));
    Serial.println("Server response body: " + response);
  } else {
    Serial.printf("Error on sending Post, code: %d\n", httpresponse);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT_PULLDOWN);
  digitalWrite(ledPin, LOW);

  // Start Wi-Fi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected successfully!");
  Serial.print("Assigned IP: ");
  Serial.println(WiFi.localIP());

  attachInterrupt(digitalPinToInterrupt(pirPin), handleMotion, RISING);
  Serial.println("System armed. Waiting for trigger...");
}

void loop() {
  if (motionDetected) {
    digitalWrite(ledPin, HIGH);
    Serial.println("\n[!] Interrupt triggered.");

    sendPostRequest();

    delay(3000);
    digitalWrite(ledPin, LOW);
    motionDetected = false;
    Serial.println("System re-armed.");
  }
}