#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <secrets.h>

constexpr uint8_t LED_PIN = 5;
constexpr const char* SERVER_URL = "http://192.168.0.103:3000";
constexpr uint32_t TELEMETRY_INTERVAL_MS = 1000;
constexpr uint16_t HTTP_TIMEOUT_MS = 2000;

uint32_t lastTelemetry = 0;
bool ledOn = false;
bool serverReachable = false;

void connectWiFi();
void sendTelemetry();
void setLed(bool on);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  setLed(false);

  connectWiFi();
}


void loop() {
  uint32_t now = millis();

  if (now - lastTelemetry >= TELEMETRY_INTERVAL_MS) {
    lastTelemetry = now;
    sendTelemetry();
  }
}


void connectWiFi() {
  Serial.printf("Connecting to %s", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected, ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

void setLed(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  ledOn = on;
  Serial.printf("LED %s\n", on ? "on" : "off");
}

void sendTelemetry() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi, skip");
    return;
  }

  JsonDocument request;
  request["uptimeMs"] = millis();
  request["rssi"] = WiFi.RSSI();
  request["led"] = ledOn;

  String requestBody;
  serializeJson(request, requestBody);

  HTTPClient http;
  http.begin(String(SERVER_URL) + "/api/telemetry");
  http.setConnectTimeout(HTTP_TIMEOUT_MS);
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  int statusCode = http.POST(requestBody);

  if (statusCode <= 0) {
    Serial.printf("Request failed, error: %s\n", http.errorToString(statusCode).c_str());
    http.end();
    serverReachable = false;
    return;
  }

  String responseBody = http.getString();
  http.end();

  if (statusCode != HTTP_CODE_OK) {
    Serial.printf("HTTP %d, body: %s\n", statusCode, responseBody.c_str());
    return;
  }

  if (!serverReachable) {
    serverReachable = true;
    Serial.println("Server connected");
  }

  JsonDocument response;
  DeserializationError error = deserializeJson(response, responseBody);

  if (error) {
    Serial.printf("JSON parse error: %s\n", error.c_str());
    return;
  }

  bool ledWanted = response["led"] | ledOn;

  if (ledWanted != ledOn) {
    setLed(ledWanted);
  }
}
