#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>

// Wi-Fi Credentials
const char* ssid = "rishabh";
const char* password = "12345678";

ESP8266WebServer server(80);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Per-servo custom HIGH and REST positions
const int SERVO_HIGH[6] = {500, 510, 520, 350, 540, 550};
const int SERVO_REST[6] = {210, 215, 220, 225, 230, 235};

const int RESET_DELAY = 600;

// Move servo to its HIGH position
void moveServoHigh(int index) {
  pwm.setPWM(index, 0, SERVO_HIGH[index]);
}

// Move servo to its REST position
void moveServoRest(int index) {
  pwm.setPWM(index, 0, SERVO_REST[index]);
}

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

  // Show matrix
  Serial.println("[Matrix]");
  Serial.println(pattern[0] + String(" ") + pattern[3]);
  Serial.println(pattern[1] + String(" ") + pattern[4]);
  Serial.println(pattern[2] + String(" ") + pattern[5]);

  // Move each servo based on pattern
  Serial.println("[Action] Moving servos based on pattern...");
  for (int i = 0; i < 6; i++) {
    if (pattern[i] == '1') {
      Serial.println("[Servo " + String(i) + "] → HIGH");
      moveServoHigh(i);
    } else {
      Serial.println("[Servo " + String(i) + "] → REST");
      moveServoRest(i);
    }
  }

  // Hold for a moment
  delay(RESET_DELAY);

  // Reset all servos to REST
  Serial.println("[Action] Resetting all servos to REST...");
  for (int i = 0; i < 6; i++) {
    moveServoRest(i);
  }

  Serial.println("[Success] Pattern complete and reset.\n");
  server.send(200, "text/plain", "Pattern applied and reset");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[Setup] Starting system...");

  Wire.begin(4, 5); // SDA = D2, SCL = D1
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);

  Serial.println("[PWM] PCA9685 initialized");

  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Connecting to " + String(ssid));
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++retry > 30) {
      Serial.println("\n[Error] WiFi connection failed.");
      return;
    }
  }

  Serial.println("\n[WiFi] Connected!");
  Serial.print("[IP] ");
  Serial.println(WiFi.localIP());

  // Move servos to REST at startup
  Serial.println("[Setup] Resetting all servos to REST position...");
  for (int i = 0; i < 6; i++) {
    moveServoRest(i);
    delay(100);
  }

  server.on("/braille", HTTP_POST, handleBraille);
  server.begin();
  Serial.println("[Server] Web server started.");
}

void loop() {
  server.handleClient();
}