#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTTYPE DHT11

const byte Buzzer = 9;
const byte DHTPIN = 7;
const byte CO2Tx = 2;
const byte CO2Rx = 3;
const byte ACK = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial co2meter(CO2Tx, CO2Rx);

const int threshold = 1100;
unsigned int preheat = 10; //loops
const unsigned long inact_time = 3600000; //millis
unsigned long enter_time = 0;
unsigned long current_time = 0;
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char response[9];
unsigned long ppm = 0;

enum States { Normal, Emergency, Inactive};
States state = Normal;

void setup() {
  lcd.init(); //16col, 2row, backlight default
  dht.begin();
  for(int i = 0; i < 3; i++) {
    lcd.backlight();
    delay(200);
    lcd.noBacklight();
    delay(200);
    co2meter.begin(9600);
  }
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CO2 Detector");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Hello, Maker!");
  delay(3000);
  lcd.clear();
}

void loop() {
  switch (state)
  {
    case Normal:
      standard_procedure();
      break;
    case Inactive:
      inactive_period();
      break;
    case Emergency:
      emergency_beep();
      break;    
  }
  delay(10000);
}


void standard_procedure()
{
  co2meter.write(cmd, 9);
  co2meter.readBytes(response, 9);
  unsigned int responseHIGH = (unsigned int)response[2];
  unsigned int responseLOW = (unsigned int)response[3];
  ppm = (256*responseHIGH)+responseLOW;
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  delay(250);
  if (isnan(h) || isnan(t))
  {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  if(preheat > 0)
  {
    lcd.print("Preheating ");
    lcd.print(preheat);
    preheat--;
  }
  else
  {
    lcd.print(ppm);
    if(ppm > threshold)
    {
      state = Emergency;
      enter_time = millis();
    }
    else
    {
      state = Normal;  
    }
    lcd.print("ppm");  
  }
  lcd.setCursor(0, 1);
  lcd.print( t);
  lcd.print("*C  ");
  lcd.print( h);
  lcd.print("%");
}

void emergency_beep()
{
  for (int j=0; j < 3; j++)
  {
    for (int i=0; i < 4; i++)
    {
      tone(Buzzer, 440, 200);
      delay(600);
      noTone(Buzzer);
    }
    delay(1000);
  }
  state = Inactive;
}

void inactive_period()
{
  co2meter.write(cmd, 9);
  co2meter.readBytes(response, 9);
  unsigned int responseHIGH = (unsigned int)response[2];
  unsigned int responseLOW = (unsigned int)response[3];
  ppm = (256*responseHIGH)+responseLOW;
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  delay(250);
  if (isnan(h) || isnan(t))
  {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  lcd.print(ppm);
  lcd.print("ppm");  
  lcd.setCursor(0, 1);
  lcd.print( t);
  lcd.print("*C  ");
  lcd.print( h);
  lcd.print("%");  
  current_time = millis();
  if( (current_time - enter_time) > inact_time)
  {
    state = Normal;  
  }
  else
  {
    state = Inactive;  
  }
}
