#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11

#define Do 262
#define Re 294
#define Mi 330
#define Fa 349
#define So 392
#define La 440
#define Si 494
#define Stop 0

const byte Buzzer = 8;
const byte DHTPIN = 7;
const byte CO2Tx = 2;
const byte CO2Rx = 3;
const byte ACK = 10;
// Set the pins on the I2C chip used for LCD connections: addr, col, row
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial co2meter(2, 3);

const int threshold = 1550;
const int lowthreshold = 1300;
int preheat = 100;
int acktime = 600;
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char response[9];
unsigned long ppm = 0;

int melody[48] = {So, Mi, Mi, Stop, Fa, Re, Re, Stop, Do, Re, Mi, Fa, So, So, So, Stop,
                  Re, Re, Re, Re, Re, Mi, Fa, Stop, Mi, Mi, Mi, Mi, Mi, Fa, So, Stop,
                  So, Mi, Mi, Stop, Fa, Re, Re, Stop, Do, Mi, So, So, Do, Stop, Stop, Stop};

enum States { Normal,  Emergency, Acknowledgement};
States state = Normal;

void setup() {
  pinMode(Buzzer, OUTPUT);
  pinMode(ACK, INPUT);
  lcd.init(); //16col, 2row, backlight default
  dht.begin();
  //blink 3 times
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // backlight on
    delay(200);
    lcd.noBacklight(); // backlight off
    delay(200);
    co2meter.begin(9600);
  }
  lcd.backlight();

  //startup message
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("CO2 Terminaor");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Hello, Maker!");
  delay(3000);
  lcd.clear();
}

void loop() {

  switch(state)
  {
    case Normal :
      standard_procedure();
      break;

    case Emergency :
      beeSong();
      exception_procedure();
      break;

    case Acknowledgement :
      exception_procedure();
      if(acktime)
      {
        --acktime;
      }
      else
      {
        state = Normal; 
      }
      break;
  }

}

void standard_procedure()
{
  delay(10000);
  co2meter.write(cmd, 9);
  co2meter.readBytes(response, 9);
  unsigned int responseHIGH = (unsigned int)response[2];
  unsigned int responseLOW = (unsigned int)response[3];
  ppm = (256*responseHIGH)+responseLOW;
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
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
};

void exception_procedure()
{
  delay(5000);
  co2meter.write(cmd, 9);
  co2meter.readBytes(response, 9);
  unsigned int responseHIGH = (unsigned int)response[2];
  unsigned int responseLOW = (unsigned int)response[3];
  ppm = (256*responseHIGH)+responseLOW;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
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
    lcd.print("ppm");  
    if(ppm < lowthreshold)
    {
      state = Normal;
      return; 
    }
  }
  lcd.setCursor(0, 1);
  lcd.print( t);
  lcd.print("*C  ");
  lcd.print( h);
  lcd.print("%");
};

void beeSong()
{
  lcd.setCursor(0, 1);
  lcd.print("**W: Press ACK. ");
  for(int i=0; i<48; i++)
  {
    tone(Buzzer, melody[i], 450);
    delay(500);
    if(digitalRead(ACK) == HIGH)
    {
      continue;
    }
    else
    {
      state = Acknowledgement;
      acktime = 600;
      return;
    }
  }
};
