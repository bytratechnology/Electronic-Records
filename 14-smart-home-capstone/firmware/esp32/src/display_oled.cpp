#include "display_oled.h"
#include "config.h"
#include "pins.h"

#if ENABLE_OLED

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool displayReady = false;

void initDisplay() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  displayReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  if (!displayReady) {
    Serial.println("[oled] not found — continuing without display");
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Smart Home");
  display.println("Starting...");
  display.display();
}

void updateDisplay(const SensorData& sensors,
                   const DeviceState& devices,
                   const SystemSettings& settings,
                   bool wifiOk,
                   bool firebaseOk) {
  if (!displayReady) {
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.print("Smart Home  ");
  display.println(isManualMode(settings) ? "MAN" : "AUTO");

  if (sensors.dhtOk) {
    display.print(sensors.temperature, 1);
    display.print("C  ");
    display.print(sensors.humidity, 0);
    display.println("%");
  } else {
    display.println("DHT: ---");
  }

  display.print("Light ");
  display.print(sensors.light);
  display.print(sensors.motion ? "  MOTION" : "  still");
  display.println();

  display.print("Gas ");
  display.print(sensors.gas);
  if (sensors.gas > settings.gasThreshold) {
    display.println("  ALERT");
  } else {
    display.println("  OK");
  }

  display.print("L:");
  display.print(devices.light ? "ON " : "off ");
  display.print("F:");
  display.print(devices.fan ? "ON " : "off ");
  display.print("R:");
  display.println(devices.relay ? "ON" : "off");

  display.print(wifiOk ? "WiFi OK  " : "WiFi --  ");
  display.println(firebaseOk ? "FB OK" : "FB --");

  display.display();
}

#else

void initDisplay() {
  Serial.println("[oled] disabled in config.h");
}

void updateDisplay(const SensorData&,
                   const DeviceState&,
                   const SystemSettings&,
                   bool,
                   bool) {}

#endif
