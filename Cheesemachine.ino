#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TEMP_READ_DELAY 800 //can only read digital temp sensor every ~750ms
#define dutycycle 8000
#define oncycle 2000
#define RELAY_PIN 7
#define TEMP_PROBE_PIN 2
#define TEMP_PROBE_GND 3
#define TEMP_PROBE_VCC 4

double setpoint = 32;
double temperature;
int relayMode = 0; //Aus = 0, Halb = 1, Ganz = 2

bool dutystate;
bool relayState;

OneWire oneWire(TEMP_PROBE_PIN);
DallasTemperature temperatureSensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long lastTempUpdate; //tracks clock time of last temp update
unsigned long lastRelayUpdate; //tracks clock time of last temp update


bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = temperatureSensors.getTempCByIndex(0); //get temp reading
    lastTempUpdate = millis();
    temperatureSensors.requestTemperatures(); //request reading for next time
    Serial.println(temperature);
    return true;
  }
  return false;
}//void updateTemperature

void setup() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
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
  while (!updateTemperature()) {} //wait until temp sensor updated
}

void heizbestimmung() {
  if ((setpoint - temperature) >= 10.0 ) {
    //Do something
    relayMode = 2;
  } else if ((setpoint - temperature) >= 1.0) {
    //Do something
    relayMode = 1;
  } else {
    //Do something
    relayMode = 0;
  }
}

void relayCycle() {
  if (relayMode == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN, LOW);
    delay(400);

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
  } else if (temperature >= 0){
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.print(temperature);
  } else if (temperature == -127){
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
