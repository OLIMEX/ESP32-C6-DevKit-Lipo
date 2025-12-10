/*
   Olimex ESP32-C6-DevKit-LiPo WiFi 6 Demo (2.4 GHz only)
   -------------------------------------------------------------
    • LED1 (GPIO15, active LOW)
         - LED behavior ON: blink patterns (fast after scan, slow idle)
         - LED behavior OFF: LED stays OFF
    • BOOT1 button (GPIO9, active LOW, internal pull-up)
         - Toggle of LED1 (edge-detected)
    • Automatic 2.4 GHz WiFi scan every 10 s
*/

#include <WiFi.h>

// GPIO definitions
#define USER_LED 15     // Active LOW
#define USER_BTN 9      // Active LOW, internal pull-up

// Timing
unsigned long lastScan = 0;
const unsigned long scanInterval = 10000;

// LED behavior toggle
bool ledEnabled = true;

// LED state machine
unsigned long lastBlink = 0;
bool ledState = false;

// Snappy button debounce state
bool btnStable = HIGH;
bool btnLastReported = HIGH;
unsigned long btnLastChange = 0;
const unsigned long debounceMs = 50;

// Helpers for LED (active low)
inline void ledOn()  { digitalWrite(USER_LED, LOW); }
inline void ledOff() { digitalWrite(USER_LED, HIGH); }

// WiFi scan
void performScan() {
  Serial.println();
  Serial.println("Scanning (2.4 GHz)...");
  Serial.println("----------------------------------------------");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);

  int n = WiFi.scanNetworks(false, true); // blocking scan
  if (n < 0) {
    Serial.println("Scan failed.");
    return;
  }

  Serial.printf("Found %d networks\n", n);
  for (int i = 0; i < n; i++) {
    int ch = WiFi.channel(i);
    Serial.printf("[%2d] SSID: %-32s RSSI: %3d dBm  CH:%2d  ENC:%d\n",
                  i,
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i),
                  ch,
                  WiFi.encryptionType(i));
  }

  WiFi.scanDelete();
}

// LED state machine: non-blocking
void updateLed() {
  if (!ledEnabled) {
    ledOff();
    return;
  }

  unsigned long now = millis();
  unsigned long sinceScan = now - lastScan;

  // Fast blink 4 Hz for 800 ms after scan
  if (sinceScan < 800) {
    if (now - lastBlink >= 125) {
      ledState = !ledState;
      ledState ? ledOn() : ledOff();
      lastBlink = now;
    }
    return;
  }

  // Idle slow blink 1 Hz
  if (now - lastBlink >= 500) {
    ledState = !ledState;
    ledState ? ledOn() : ledOff();
    lastBlink = now;
  }
}

// Snappy button logic: falling-edge detection with debounce
void updateButton() {
  int raw = digitalRead(USER_BTN);
  unsigned long now = millis();

  // If state changed, reset debounce timer
  if (raw != btnStable) {
    btnLastChange = now;
    btnStable = raw;
  }

  // If stable long enough, consider it valid
  if (now - btnLastChange > debounceMs) {
    // Detect falling edge: HIGH -> LOW only once
    if (btnStable == LOW && btnLastReported == HIGH) {
      // Toggle LED behavior
      ledEnabled = !ledEnabled;
      Serial.println(ledEnabled ? "LED behavior: ENABLED" : "LED behavior: DISABLED");
      btnLastReported = LOW;
    }

    // Detect release (LOW -> HIGH)
    if (btnStable == HIGH && btnLastReported == LOW) {
      btnLastReported = HIGH;
    }
  }
}

void setup() {
  pinMode(USER_LED, OUTPUT);
  pinMode(USER_BTN, INPUT_PULLUP);

  ledOff();
  delay(20);

  Serial.begin(115200);
  delay(300);

  Serial.println();
  Serial.println("Olimex ESP32-C6-DevKit-LiPo WiFi 6 Demo");
  Serial.println("Snappy button toggle for LED behavior.");
  Serial.println("-----------------------------------------------------");

  performScan();
  lastScan = millis();
}

void loop() {
  updateButton();

  if (millis() - lastScan >= scanInterval) {
    lastScan = millis();
    performScan();
  }

  updateLed();
}
