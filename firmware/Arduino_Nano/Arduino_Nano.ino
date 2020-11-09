// ***************************** НАСТРОЙКИ *****************************

#define IR_PIN 2          //пин для ик датчика
#define BTN_PIN 3         // пин кнопки
#define DHTPIN 5
#define echoPin 6
#define trigPin 7
#define POINT 8
#define RX 12              //фил
#define TX 13              //зел
#define PH_RES A1
//---------------------------------БИБЛИОТЕКИ-----------------------------
#include <SoftwareSerial.h>
SoftwareSerial mySerial(RX, TX); // RX, TX

#include "IRLremote.h"
CHashIR IRLremote;
uint32_t IRdata;



/*#define PARSE_AMOUNT 3         // число значений в массиве, который хотим получить
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга
#include "GParsingStream.h"*/

#include <NewPing.h>
#define MAX_DISTANCE 250
NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

#include "stDHT.h"
#define DHTTYPE DHT22   // Указываем, какой тип датчика мы используем
DHT dht(DHTTYPE);

#include <Wire.h>
#include <Adafruit_BMP085.h>   // библиотека датчика
Adafruit_BMP085 bmp; //объявить датчик с именем bmp

// ----- КНОПКИ ПУЛЬТА--------
#define BUTT_UP     0xF39EEBAD
#define BUTT_DOWN   0xC089F6AD
#define BUTT_LEFT   0xE25410AD
#define BUTT_RIGHT  0x14CE54AD
#define BUTT_OK     0x297C76AD
#define BUTT_1      0x4E5BA3AD
#define BUTT_2      0xE51CA6AD
#define BUTT_3      0xE207E1AD
#define BUTT_4      0x517068AD
#define BUTT_5      0x1B92DDAD
#define BUTT_6      0xAC2A56AD
#define BUTT_7      0x5484B6AD
#define BUTT_8      0xD22353AD
#define BUTT_9      0xDF3F4BAD
#define BUTT_0      0xF08A26AD
#define BUTT_STAR   0x68E456AD
#define BUTT_HASH   0x151CD6AD




unsigned long info_timer;


byte mov;
byte send_brig;

int delta, chance;
unsigned long weather_timer;
unsigned long bar, aver_pressure, pressure_array[6], time_array[6];
unsigned long sumX, sumY, sumX2, sumXY;
float a;
int BRIGHTNESS;

// ------------------------------ ДЛЯ РАЗРАБОТЧИКОВ --------------------------------
void setup() {

  Serial.begin(115200);
  mySerial.begin(115200);

  IRLremote.begin(IR_PIN);

  bmp.begin(BMP085_ULTRAHIGHRES);  // включить датчик

  pinMode(DHTPIN, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(POINT, OUTPUT);

  bar = aver_sens();          // найти текущее давление по среднему арифметическому
  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = bar;  // забить весь массив текущим давлением
    time_array[i] = i;             // забить массив времени числами 0 - 5
  }
  Serial.println(bar);
}


void loop() {
  Weather();
  remoteTick();
  info();
}


void info() {
  int distance;
  int P, val;
  byte t, h;

  if (millis() - info_timer > 100) {
    t = dht.readTemperature(DHTPIN);
    h = dht.readHumidity(DHTPIN);
    P = (bmp.readPressure()/100) * 0.750064, 2;
    distance = sonar.ping_median() / US_ROUNDTRIP_CM;
    val = analogRead(PH_RES);

    mySerial.println("$" + String(t) + " " + String(h) + " " + String(P) + " " + String(distance) + " " + String(val) + " " + String(mov) + " " + String(send_brig)
                     + " " + String(chance) + ";");
    mov = 0;
    send_brig = 0;
    info_timer = millis();
  }
}
