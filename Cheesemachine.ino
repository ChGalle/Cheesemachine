//  Mit dieser Software lässt sich ein Einkochkessel mit einem Solid-State-Relais, und einem DS18B20 Temperatursensor steuern,
//  so dass dieser möglichst genau eine Temperatur von 32°C erreicht und hält.


#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TEMP_READ_DELAY 800 //der Sensor kann nur ca. alle 750ms einen Messwert liefern
#define dutycycle 400 // Ruhezeit im Pulsebetrieb
#define oncycle 100 // Einschaltzeit im Pulsbetrieb
#define RELAY_PIN 7 //Relaispin
#define TEMP_PROBE_PIN 2 // DS18B20 DataPin
#define TEMP_PROBE_GND 3 // DS18B20 GND PIN
#define TEMP_PROBE_VCC 4 // DS18B20 VCC PIN

double setpoint = 32; // Gewünschte Zieltemperatur
double temperature; // Variable für die Temperatur
int relayMode = 0; //Aus = 0, Halb = 1, Ganz = 2

// Initialisiere die Bibliotheken
OneWire oneWire(TEMP_PROBE_PIN);
DallasTemperature temperatureSensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 20, 4); //LCD mit 20x4 Zeichen

unsigned long lastTempUpdate; //Letztes Temperaturupdate


bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = temperatureSensors.getTempCByIndex(0); //hole Messung ab
    lastTempUpdate = millis();
    temperatureSensors.requestTemperatures(); //Beauftrage nächste Messung
    return true;
  }
  return false;
}

void setup() {
  lcd.init();                      // Initialisiere LCD
  lcd.backlight();                 // Schalte LCD Hintergrundbeleuchtung ein
  lcd.home();
  lcd.print("Cheesy-Peasy-Machine");
  lcd.setCursor(0, 2);
  lcd.print("  Bergkaeseapparat  ");
  delay(2000);
  lcd.clear();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TEMP_PROBE_GND, OUTPUT);
  pinMode(TEMP_PROBE_VCC, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(TEMP_PROBE_GND, LOW);
  digitalWrite(TEMP_PROBE_VCC, HIGH);

  temperatureSensors.begin();
  temperatureSensors.requestTemperatures();
  while (!updateTemperature()) {} //Warte bis erste Temperatur vorliegt...
}

void heizbestimmung() {
  if ((setpoint - temperature) >= 10.0 ) {
    //Volle Heizleistung
    relayMode = 2;
  } else if ((setpoint - temperature) >= 1.0) {
    //Reduzierte Heizleistung
    relayMode = 1;
  } else {
    //Beende Heizen
    relayMode = 0;
  }
}

void relayCycle() {
  if (relayMode == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    delay(oncycle);
    digitalWrite(RELAY_PIN, LOW);
    delay(dutycycle);

  }   else if (relayMode == 0) {
    //Do something
    digitalWrite(RELAY_PIN, LOW);;
  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void lcdAusgabe() {
  lcd.setCursor(0, 0);
  lcd.print("Aktuelle Temperatur:");

  if (temperature >= 10) {
    lcd.setCursor(0, 1);
    lcd.print(temperature);
  } else if (temperature >= 0) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.print(temperature);
  } else if (temperature == -127) {
    lcd.setCursor(0, 1);
    lcd.print("ERR    ");

  }

}

void loop() {
  updateTemperature();
  heizbestimmung();
  relayCycle();
  lcdAusgabe();
}
