#include "ESP8266WiFi.h"

#include "ThingSpeak.h"

#include "EmonLib.h"

#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
EnergyMonitor emon1;
const byte thePin = A0;
int relay = D5;
int Irms;
//long int maxpower;
const char * ssid = "POCO X3";
const char * password = "12345678";
WiFiClient client;
unsigned long myChannelNumber = 2480435;
const char * myWriteAPIKey = "NNYFLA8G6O10FVAW";
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
void setup() {
  pinMode(relay, OUTPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Electricity Theft");
  lcd.setCursor(1, 1);
  lcd.print("Detection");
  delay(2000);
  lcd.clear();
  //Serial.println("Enter the max power");
  //while (Serial.available() == 0) {}
  //int maxpower = Serial.parseInt();
  //lcd.print("Maxpower:");
  //lcd.print(maxpower);
  //delay(5000);
  //lcd.clear();
  emon1.current(thePin, 111.1);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}
void loop() {

  Irms = emon1.calcIrms(1480);
  int power = Irms * 240.0;
  Serial.print(power);
  Serial.print(" ");
  Serial.println(Irms);

  if (power > 20000) {
    digitalWrite(relay, HIGH);
    lcd.setCursor(1, 0);
    lcd.print("OL DETECTED");
    delay(2000);
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(1, 0);
    lcd.print("Current(mA):");
    lcd.print(Irms);
    lcd.setCursor(1, 1);
    lcd.print("Power(W)");
    lcd.print(power);
    delay(2000);
    lcd.clear();
  } else {
    digitalWrite(relay, LOW);
    lcd.backlight();
    lcd.setCursor(1, 0);
    lcd.print("Current(mA):");
    lcd.print(Irms);
    lcd.setCursor(1, 1);
    lcd.print("Power(W)");
    lcd.print(power);
    delay(2000);
    lcd.clear();

  }
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
      // IrmscheckSerial();
    }
    ThingSpeak.setField(1, Irms);
    ThingSpeak.setField(2, power);
    //ThingSpeak.setField(3, pres);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }

}
void checkSerial() {
  if (Serial.available()) {
    String buff = "";
    while (Serial.available()) {
      delay(1);
      char c = Serial.read();
      buff += c;
    } // end while
    if (buff.length() >= 1) {
      Serial.println("input: " + buff);
      buff.replace("\r", "");
      buff.replace("\n", "");
      float calibFac = buff.toFloat(); // buff.toDouble();
      emon1.current(thePin, calibFac);
    }
  }
}
