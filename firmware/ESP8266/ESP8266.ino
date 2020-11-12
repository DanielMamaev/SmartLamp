#define LED_PIN 13
#define BTN_PIN 12
#define CLOCK   14
#define RESET   16
#define SOFT_TX 4
#define SOFT_RX 5
#define SOUND_R A0

#define NUM_LEDS 16        // количество светодиодов (данная версия поддерживает до 410 штук)
#define CURRENT_LIMIT 950  // лимит по току в МИЛЛИАМПЕРАХ, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define NUM_STRIPS 4        // количество отрезков ленты (в параллели)
int BRIGHTNESS = 255;    // яркость по умолчанию (0 - 255)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <PubSubClient.h>
//#include <DNSServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>
WiFiManager wifiManager;

boolean indi_mode_bool = true;
byte r, g, b;
boolean sign_check = false;
byte this_mode = 6;
byte ColorMusic = 1;
boolean auto_play_on_off = true;
unsigned long send_timer, timer, reconect_timer, send_nano_timer;
byte t, h;
int P;
int light_sen, distance;
int chance;
int m_d[] = {1000, 1000};
boolean ONstate = false;

unsigned long main_timer, color_timer, rainbow_timer, info_timer, running_timer, auto_timer;
byte ani_mode = 1;
int count;
float ind = (float)255 / NUM_LEDS;
//-----Режим измерения температуры и влажности
byte temp_mode = 1, hum_mode = 1, bar_mode = 1;

//-----Режим дальномера
float dis_f;
float SMOOTH_2 = 0.05;

boolean VU_check, VU_sig;
byte sign_byte = 9, vu_byte, light_byte, mov_byte;

//------------------------------MQTT-------------------
#define BUFFER_SIZE 100
const char *mqtt_server = "a"; // Имя сервера MQTT                  "postman.cloudmqtt.com"
int mqtt_port = 1; // Порт для подключения к серверу MQTT     17110
const char *mqtt_user = "a"; // Логин от сервер                     "obtxfdbf"
const char *mqtt_pass = "a"; // Пароль от сервера                   "6uD0FYaNQT5K"

// Функция получения данных от сервера
void callback(const MQTT::Publish& pub)
{
  String payload = pub.payload_string();
  String topic = pub.topic();

  //Serial.print(pub.topic()); // выводим в сериал порт название топика
  //Serial.print(" => ");
  //Serial.println(payload); // выводим в сериал порт значение полученных данных

  // проверяем из нужного ли нам топика пришли данные

  if (topic == "brig")
  {
    FastLED.setBrightness(payload.toInt());
  }

  if (topic == "color")
  {
    String hexstring = "#" + payload;
    long number = (long) strtol( &hexstring[1], NULL, 16);
    r = number >> 16;
    g = number >> 8 & 0xff;
    b = number & 0xff;
    FastLED.clear();
    this_mode = 7;
    indi_mode_bool = true;
  }

  if (topic == "move")
  {
    int mov = payload.toInt();
    if (mov == 1) STAR();
    if (mov == 2) RIGHT();
    if (mov == 3) LEFT();
    if (mov == 4) auto_play_on_off = !auto_play_on_off;
    if (mov == 5) UP();
    if (mov == 6) DOWN();
    if (mov == 7) HASH();
    if (mov == 8)sign_check = true;
    if (mov == 9)sign_check = false;
    if (mov == 100) fullLowPass();
    if (mov == 0) res_wifi();
  }
}
WiFiClient wclient;
PubSubClient client(wclient);//, mqtt_server, mqtt_port);

//-----------------OTA---------------------------
#include <ESP8266HTTPUpdateServer.h>
#define OTAUSER         "admin"    // Логин для входа в OTA
#define OTAPASSWORD     "admin"    // Пароль для входа в ОТА
#define OTAPATH         "/firmware"// Путь, который будем дописывать после ip адреса в браузере.
#define SERVERPORT      80         // Порт для входа, он стандартный 80 это порт http
ESP8266WebServer HttpServer(SERVERPORT);
ESP8266HTTPUpdateServer httpUpdater;

//--------------ПАРСИНГ ДАННЫХ----------------


#include "GyverButton.h"
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
#define MIN_BRIGHTNESS 5
boolean brightDirection = true;
boolean wasStep = false;
int tempBrightness;



//************************************************************* АНИМАЦИЯ ***************************************************************

// ----- настройки ленты

CRGB leds[NUM_LEDS];
// ----- отрисовка
#define MAIN_LOOP 4               // период основного цикла отрисовки (по умолчанию 5)
#define MODE_AMOUNT 7             // количество режимов

#include "GyverTimer.h"
GTimer_ms brightTimer(20);
GTimer_ms effectTimer(55);

//------------------------------------------------------ПЕРЕМЕННЫЕ ЭФФЕКТОВ-------------------------------------------
//-----------заполнение ленты по одному одинакомову цвету

byte m_rand[NUM_LEDS] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
byte id_m = 0;
unsigned long one_fr_dl_timer, ofr_timer;
boolean ofr = true;
int del_ofr = 5000;
byte ofr_br;
boolean ofr_dl;

//----------ПЛАВНОЕ ЗАТУХАНИЕ ---------

byte brig = 0;   //Плавная смена яркости
boolean brig_flag = false;
boolean go_damp = true;
boolean damp_sped_flag = true;
unsigned long damping_timer, damping_timer_speed;
int DAMPING_DELAY = 7000;
byte DAMPING_SPEED = 4;

//----------ПЛАВНОЕ ЗАПОЛНЕНИЕ ЦВЕТОМ ---------

unsigned long increase_timer, delay_timer_incr;
byte increase_ix = -1;
boolean bound_incr;
byte INCREASE_SPEED = 1000;
int INCREASE_DELAY = 9000;
boolean go_incr = true;

//-------- ДЛЯ БЕГУЩЕГО ДИОДА------

unsigned long lighter_timer;
boolean bound = false;
byte ix = 0;                 //индекс левого
byte LIGHTER_SPEED = 140;   //скорость светодиода
byte satur = 255;           //Насыщенность

//-------- ДЛЯ БЕГУЩИХ ДИОДОВ ------

byte LIGHTER_2_SPEED = 140;
byte color;                  //цвет левого светодиода
byte col1;                   //цвет правого светодиода
boolean bouncedirection = true;
boolean boun = false;
byte idex = NUM_LEDS - 1;  //начальный индекс правого светодиода
byte idexx = 0;            //начальный индекс левого светодиода

//-------------ДЛЯ СВЕТЛЯЧКОВ ----

#define MAX_SPEED 15  //СКОРОСТЬ
#define BUGS_AMOUNT 2 //КОЛ-ВО ОГОНЬКОВ
int8_t speed[BUGS_AMOUNT];
int8_t pos[BUGS_AMOUNT];
CRGB bugColors[BUGS_AMOUNT];
boolean loadingFlag = true;
unsigned long bugs_timer;

//------ДЛЯ РАНДОМНЫХ ВСПЫШЕК ----
byte num;               //Рандомный светодиод

//------ДЛЯ РАНДОМНОГО ПЛАВНОГО ОГОНЬКА----

boolean run_diod_flag = false;
boolean brightness_flag;
boolean go_one = true;
unsigned long rand_diod_timer;
int ONE_RAND_DELAY = 2000;


//------ДЛЯ РАНДОМНЫХ ПЛАВНЫХ ОГОНЬКОВ----
byte KOL_DIODS = 7;
byte kol;

//------------ ОГОНЬ-------------

CRGBPalette16 gPal;
CRGBPalette16 gPal1 = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow );
CRGBPalette16 gPal2 = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White );
CRGBPalette16 gPal3 = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Blue, CRGB::White);
CRGBPalette16 gPal4 = CRGBPalette16( CRGB::Black, CRGB::Purple, CRGB::Red, CRGB::Black );
bool gReverseDirection = false;
#define COOLING  55
#define SPARKING 120

//--------------HARD COLOR-------
unsigned long hcolor_timer;
byte HARD_COLOR;

// ----- AutoColors---
byte LIGHT_COLOR = 0;
byte COLOR_SPEED = 100;
// градиент-палитра от зелёного к красному
DEFINE_GRADIENT_PALETTE(soundlevel_gp) {
  0,    0,    255,  0,  // green
  100,  255,  255,  0,  // yellow
  150,  255,  100,  0,  // orange
  200,  255,  50,   0,  // red
  255,  255,  0,    0   // red
};
CRGBPalette32 myPal = soundlevel_gp;


DEFINE_GRADIENT_PALETTE(templevel) {
  0,    0,    0,   255, // blue
  120,  0,    255,   0, // green
  200,  255,  40,   0,  // red
  255,  255,  0,     0  // red
};
CRGBPalette32 myPal_t = templevel;

DEFINE_GRADIENT_PALETTE(humlevel) {
  0,    255,  0,     0,  // red
  160,  0,    255,   0,  // green
  255,  0,    0,     255 // blue
};
CRGBPalette32 myPal_h = humlevel;


DEFINE_GRADIENT_PALETTE(dislevel) {
  0,      255,    0,      0,   // red
  150,    255,    255,    0,  // yellow
  150,    50,    255,    0,  // green
  255,    0,      255,    0  // green
};
CRGBPalette32 myPal_d = dislevel;

DEFINE_GRADIENT_PALETTE(barlevel) {
  0,    0,    0,   255, // blue
  80,  0,    255,   0, // green
  120,  0,    255,   0, // green
  200,  255,  40,   0,  // red
  255,  255,  0,     0  // red
};
CRGBPalette32 myPal_p = barlevel;


// ----- нижний порог шумов
uint16_t LOW_PASS = 800;          // нижний порог шумов режим VU, ручная настройка
#define AUTO_LOW_PASS 0           // разрешить настройку нижнего порога шумов при запуске (по умолч. 0)
#define EEPROM_LOW_PASS 1         // порог шумов хранится в энергонезависимой памяти (по умолч. 1)
#define LOW_PASS_ADD 13           // "добавочная" величина к нижнему порогу, для надёжности (режим VU)


boolean reset_esp = false;

#include <SoftwareSerial.h>
SoftwareSerial mySerial(SOFT_RX, SOFT_TX); // RX, TX

#include <ESP8266Ping.h>
const char* remote_host = "www.google.com";
unsigned long wifi_rec_timer;
boolean ping_check;
boolean mqtt_reconnect;
boolean mqtt_run;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  EEPROM.begin(512);

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / NUM_STRIPS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  FastLED.clear();


  String ssid_w = WiFi.SSID();
  String pass_w = WiFi.psk();
  Serial.println();
  Serial.println(ssid_w);
  Serial.println(pass_w);

  if (!((ssid_w.equals("")) && (pass_w.equals("")))) {
    WiFi.begin(ssid_w, pass_w);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Wi-Fi connection failed");
      leds[NUM_LEDS - 1] = CHSV(HUE_RED, 255, 255);
      FastLED.show();
      delay(2000);
    }
    else {
      Serial.println("Wi-Fi connection success");
      leds[NUM_LEDS - 1] = CHSV(HUE_GREEN, 255, 255);
      FastLED.show();
      delay(2000);
    }
  }
  else {
    if (1 == EEPROM.read(300)) {
      leds[NUM_LEDS - 1] = CHSV(HUE_YELLOW, 255, 255);
      FastLED.show();
      delay(2000);
      EEPROM.write(300, 0);
      EEPROM.commit();
      wifiManager.autoConnect("SmartLamp");
    }
  }
  FastLED.clear();
  FastLED.show();



  //wifiManager.resetSettings();


  httpUpdater.setup(&HttpServer, OTAPATH, OTAUSER, OTAPASSWORD);
  HttpServer.begin();



  pinMode(CLOCK, OUTPUT);
  pinMode(RESET, OUTPUT);

  touch.setTimeout(300);
  touch.setStepTimeout(50);

  if (AUTO_LOW_PASS && !EEPROM_LOW_PASS) {         // если разрешена автонастройка нижнего порога шумов
    autoLowPass();
  }

  byte len_mqtt = EEPROM.read(200);
  byte len_port = EEPROM.read(201);
  byte len_user = EEPROM.read(202);
  byte len_pass = EEPROM.read(203);

  if ((len_mqtt < 33) && (len_mqtt != 0)) {
    unsigned char* buf_mqtt = new unsigned char[32];
    unsigned char* buf_port = new unsigned char[32];
    unsigned char* buf_user = new unsigned char[32];
    unsigned char* buf_pass = new unsigned char[32];

    for (byte i = 0; i < len_mqtt; i++) buf_mqtt[i] = char(EEPROM.read(i));
    buf_mqtt[len_mqtt] = '\x0';
    const char *mqtt  = (const char*)buf_mqtt;

    for (byte i = 0; i < len_port; i++) buf_port[i] = char(EEPROM.read(i + 32));
    buf_port[len_port] = '\x0';
    const char *port  = (const char*)buf_port;

    String port_char = (const char *)port;
    int port_int = port_char.toInt();

    for (byte i = 0; i < len_user; i++) buf_user[i] = char(EEPROM.read(i + 64));
    buf_user[len_user] = '\x0';
    const char *user  = (const char*)buf_user;

    for (byte i = 0; i < len_pass; i++) buf_pass[i] = char(EEPROM.read(i + 96));
    buf_pass[len_pass] = '\x0';
    const char *pass  = (const char*)buf_pass;
    mqtt_server = (const char*)mqtt;
    mqtt_port = port_int;
    mqtt_user = (const char*)user;
    mqtt_pass = (const char*)pass;

  }

  /*Serial.println(mqtt_server);
    Serial.println(mqtt_port);
    Serial.println(mqtt_user);
    Serial.println(mqtt_pass);*/
  if (Ping.ping(remote_host)) {
    check_mqtt();
    ping_check = true;
  }

  delay(100);
}

void loop() {

  touch_v();
  parsing_nano();
  Bluetooth();
  WIFI();
  mainLoop();
  indi_mode();



  if (reset_esp) {
    reset_esp = false;
    ESP.restart();
  }
}

void WIFI() {
  if (WiFi.status() == WL_CONNECTED)
  {
    if (ping_check) {
      HttpServer.handleClient(); //OTA
      MQTT_connect();
    }
    else {
      client.disconnect();
      mqtt_reconnect = true;
    }

    if (millis() - wifi_rec_timer > 30000) {
      wifi_rec_timer = millis();
      ping_check = Ping.ping(remote_host, 1);
      if (ping_check && mqtt_reconnect) {

        check_mqtt();

        mqtt_reconnect = false;
      }
    }
  }
}


void check_mqtt() {
  String s = (const char*) mqtt_server;
  String u = (const char*) mqtt_user;
  String p = (const char*) mqtt_pass;
  if (s.equals("a") || u.equals("a") || p.equals("a") || mqtt_port == 1) {
    client.disconnect();
    mqtt_run = false;
  }
  else {
    mqtt_run = true;
    client.set_server(mqtt_server, mqtt_port);
  }
}


void indi_mode() {
  if (indi_mode_bool == true) {
    int mode = this_mode;
    digitalWrite(RESET, HIGH);
    digitalWrite(RESET, LOW);
    for (int i = 0; i < mode; i++) {
      digitalWrite(CLOCK, HIGH);
      digitalWrite(CLOCK, LOW);
    }
  }
  indi_mode_bool = false;
}


void res_wifi() {
  EEPROM.write(300, 1);
  EEPROM.commit();
  delay(100);
  wifiManager.resetSettings();
  ESP.restart();
}
