#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define ADC_BAT 34   // pin donde está el divisor de la batería

Adafruit_INA219 ina219;

// Pantallas
Adafruit_SSD1306 oled1(128, 64, &Wire); // 0x3C
Adafruit_SSD1306 oled2(128, 64, &Wire); // 0x3D

float readBatteryVoltage() {
  int raw = analogRead(ADC_BAT);       
  float v_adc = (3.3f * raw) / 4095.0f;
  float v_bat = v_adc * (150.0f + 33.0f) / 33.0f; // divisor
  return v_bat;
}

float soc(float v) {
  if (v <= 11.8f) return 0.0f;
  if (v >= 13.0f) return 100.0f;
  return (v - 11.8f) * (100.0f / (13.0f - 11.8f));
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);

  ina219.begin();

  oled1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled1.clearDisplay();
  oled2.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  oled2.clearDisplay();

  analogReadResolution(12);
}

void loop() {
  float busV = ina219.getBusVoltage_V();
  float shunt = ina219.getShuntVoltage_mV() / 1000.0;
  float currentA = ina219.getCurrent_mA() / 1000.0;
  float loadV = busV + shunt;
  float power = loadV * currentA;

  float vbat = readBatteryVoltage();
  float charge = soc(vbat);

  // Pantalla 1
  oled1.clearDisplay();
  oled1.setCursor(0, 0);
  oled1.println("CARGA SOLAR");
  oled1.print("V: "); oled1.println(loadV, 2);
  oled1.print("I: "); oled1.println(currentA, 3);
  oled1.print("P: "); oled1.println(power, 2);
  oled1.display();

  // Pantalla 2
  oled2.clearDisplay();
  oled2.setCursor(0, 0);
  oled2.println("BATERIA");
  oled2.print("Vbat: "); oled2.println(vbat, 2);
  oled2.print("SOC: "); oled2.println(charge, 0);
  oled2.display();

  delay(500);
}
