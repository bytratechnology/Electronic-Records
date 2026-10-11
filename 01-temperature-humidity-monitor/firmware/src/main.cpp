#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

/*
 * Project 01 — Temperature and humidity monitor
 * Board: Arduino Uno. Wiring table: ../README.md
 *
 * Reads a DHT sensor every two seconds, shows the values on a 16x2 I2C LCD,
 * and prints CSV on Serial so the Arduino Serial Plotter can graph them.
 */

#define PIN_DHT 2
#define DHT_TYPE DHT11         // change to DHT22 for the white AM2302 sensor
#define LCD_ADDRESS 0x27       // some I2C backpacks answer on 0x3F instead
#define READ_INTERVAL_MS 2000  // DHT sensors cannot be read faster than this
#define MAX_FAILED_READS 3

static DHT dht(PIN_DHT, DHT_TYPE);
static LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

static float minTemperature = NAN;
static float maxTemperature = NAN;
static uint8_t failedReads = 0;
static unsigned long lastReadMs = 0;

static void showReading(float temperature, float humidity) {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print(static_cast<char>(223));  // degree symbol in the HD44780 font
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%  ");

  lcd.setCursor(0, 1);
  lcd.print("Lo ");
  lcd.print(minTemperature, 1);
  lcd.print(" Hi ");
  lcd.print(maxTemperature, 1);
  lcd.print("  ");
}

static void showSensorError() {
  lcd.setCursor(0, 0);
  lcd.print("Sensor error    ");
  lcd.setCursor(0, 1);
  lcd.print("Check DHT wiring");
}

static void trackMinMax(float temperature) {
  if (isnan(minTemperature) || temperature < minTemperature) {
    minTemperature = temperature;
  }
  if (isnan(maxTemperature) || temperature > maxTemperature) {
    maxTemperature = temperature;
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.print("Temp & Humidity");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Header line for the Serial Plotter legend.
  Serial.println("temperature_c,humidity_percent");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastReadMs < READ_INTERVAL_MS) {
    return;
  }
  lastReadMs = now;

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    // One bad read is normal for a DHT; only report after several in a row.
    if (failedReads < MAX_FAILED_READS) {
      failedReads++;
    }
    if (failedReads >= MAX_FAILED_READS) {
      showSensorError();
      Serial.println("# DHT read failed");
    }
    return;
  }

  failedReads = 0;
  trackMinMax(temperature);
  showReading(temperature, humidity);

  Serial.print(temperature, 1);
  Serial.print(",");
  Serial.println(humidity, 0);
}
