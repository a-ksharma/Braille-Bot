#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>

// Wi-Fi Credentials
const char* ssid = "Ayush";
const char* password = "ayush028";

// Web server
ESP8266WebServer server(80);

// PCA9685 Servo Driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Pulse lengths for servo angles (~30° range)
// Pulse lengths for servo angles (~40° range)
const int SERVO_REST = 205;       // ~0°
const int SERVO_ACTIVE = 290;     // ~40°
const int RESET_DELAY = 600;      // milliseconds

void handleBraille() {
  Serial.println("\n[Request] /braille endpoint hit");

  if (!server.hasArg("plain")) {
    Serial.println("[Error] No body found in request.");
    server.send(400, "text/plain", "No pattern received");
    return;
  }

  String body = server.arg("plain");
  Serial.println("[Info] Received body: " + body);

  StaticJsonDocument<100> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Serial.println("[Error] JSON deserialization failed: " + String(error.c_str()));
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  String pattern = doc["pattern"];
  Serial.println("[Info] Parsed pattern: " + pattern);

  if (pattern.length() != 6) {
    Serial.println("[Error] Pattern must be exactly 6 characters.");
    server.send(400, "text/plain", "Pattern must be 6 characters");
    return;
  }

  // Display 2x3 Braille matrix
  Serial.println("[Info] Braille Matrix:");
  Serial.println(pattern[0] + String(" ") + pattern[3]);
  Serial.println(pattern[1] + String(" ") + pattern[4]);
  Serial.println(pattern[2] + String(" ") + pattern[5]);

  // Move servos based on pattern
  Serial.println("[Action] Moving servos based on pattern...");
  for (int i = 0; i < 6; i++) {
    if (pattern[i] == '1') {
      Serial.println("[Servo " + String(i) + "] → ACTIVE");
      pwm.setPWM(i, 0, SERVO_ACTIVE);
    } else {
      Serial.println("[Servo " + String(i) + "] → REST");
      pwm.setPWM(i, 0, SERVO_REST);
    }
  }

  delay(RESET_DELAY); // Wait before resetting

  // Reset all servos
  Serial.println("[Action] Resetting all servos to REST position...");
  for (int i = 0; i < 6; i++) {
    pwm.setPWM(i, 0, SERVO_REST);
  }

  Serial.println("[Success] Pattern applied and reset.\n");
  server.send(200, "text/plain", "Pattern applied and reset");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[Setup] Starting system...");

  Wire.begin(4, 5); // SDA = D2, SCL = D1
  Serial.println("[Info] I2C initialized on D2 (SDA), D1 (SCL)");

  pwm.begin();
  pwm.setPWMFreq(50); // 50Hz for standard servos
  delay(10);
  Serial.println("[Info] PCA9685 initialized at 50Hz");

  // Wi-Fi Setup
  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Connecting to ");
  Serial.print(ssid);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retry++;
    if (retry > 30) {
      Serial.println("\n[Error] Failed to connect to Wi-Fi. Check credentials or signal strength.");
      return;
    }
  }

  Serial.println("\n[WiFi] Connected!");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  // Web server
  server.on("/braille", HTTP_POST, handleBraille);
  server.begin();
  Serial.println("[Server] HTTP server started on port 80");
}

void loop() {
  server.handleClient();
}