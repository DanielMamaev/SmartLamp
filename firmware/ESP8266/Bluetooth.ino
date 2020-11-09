String strData = "";
boolean recivFlag;
boolean name_mqtt = false;
byte tic_name_mqtt = 0;

String mqtt = "";
String port = "";
String username = "";
String password = "";
unsigned char* buf = new unsigned char[64];

void Bluetooth() {

  while (mySerial.available() > 0) {         // ПОКА есть что то на вход
    strData += (char)mySerial.read();        // забиваем строку принятыми данными
    recivFlag = true;                   // поднять флаг что получили данные
    delay(3);                              // ЗАДЕРЖКА. Без неё работает некорректно!
  }

  if (recivFlag) { // если данные получены

    String temp_strData = strData;
    temp_strData.remove(1);

    if (temp_strData.equals("b")) {
      temp_strData = strData;
      temp_strData.replace("b", "");
      temp_strData.toInt();
      FastLED.setBrightness(temp_strData.toInt());
    }

    if (temp_strData.equals("c")) {
      temp_strData = strData;
      temp_strData.replace("с", "");

      String hexstring = "#" + temp_strData;
      long number = (long) strtol( &hexstring[1], NULL, 16);
      r = number >> 16;
      g = number >> 8 & 0xff;
      b = number & 0xff;
      FastLED.clear();
      this_mode = 7;
      indi_mode_bool = true;
    }

    if (temp_strData.equals("m")) {
      name_mqtt_server();
    }

    int byteData = strData.toInt();
    if (byteData == 1) STAR();
    if (byteData == 2) RIGHT();
    if (byteData == 3) LEFT();
    if (byteData == 4) auto_play_on_off = !auto_play_on_off;
    if (byteData == 5) UP();
    if (byteData == 6) DOWN();
    if (byteData == 7) HASH();
    if (byteData == 8) sign_check = true;
    if (byteData == 9) sign_check = false;
    if (byteData == 100) fullLowPass();

    if (byteData == 101) {
      client.disconnect();
      WiFi.mode(WIFI_OFF);//WIFI_AP, WIFI_STA, WIFI_AP_STA, WIFI_OFF
    }
    if (byteData == 102) {
      wifiManager.autoConnect("Lamp");
    }

    Serial.println(strData);               // вывести
    strData = "";                          // очистить
    recivFlag = false;                  // опустить флаг
  }
  send_data_blu();
}



void name_mqtt_server() {
  String first = "";
  String last = strData;
  byte index_temp;
  last.remove(0, 1);

  for (int i = 0; i < 3; i++) {
    index_temp = last.indexOf(";");
    first = last;
    first.remove(index_temp);
    switch (i) {
      case 0: mqtt = first;
        break;
      case 1: port = first;
        break;
      case 2: username = first;
        break;
    }
    last.replace(first + ";", "");
  }
  password = last;

  Serial.println(mqtt);
  Serial.println(port);
  Serial.println(username);
  Serial.println(password);




  EEPROM.begin(512);

  if (mqtt != "") {
    EEPROM.write(200, mqtt.length());
    EEPROM.write(201, port.length());
    EEPROM.write(202, username.length());
    EEPROM.write(203, password.length());

    mqtt.getBytes(buf, mqtt.length() + 1);
    for (byte i = 0; i < mqtt.length(); i++)
      EEPROM.write(i, buf[i]);

    port.getBytes(buf, port.length() + 1);
    for (byte i = 0; i < port.length(); i++)
      EEPROM.write(i + 32, buf[i]);

    username.getBytes(buf, username.length() + 1);
    for (byte i = 0; i < username.length(); i++)
      EEPROM.write(i + 64, buf[i]);

    password.getBytes(buf, password.length() + 1);
    for (byte i = 0; i < password.length(); i++)
      EEPROM.write(i + 96, buf[i]);

    EEPROM.commit();
    EEPROM.end();
  }
  reset_esp = true;
}


unsigned long time_send;

void send_data_blu() {
  if (millis() - time_send > 1000) {
    time_send = millis();

    //----СИГНАЛИЗАЦИЯ
    if (sign_check) {
      m_d[0] = distance;
      int del = m_d[1] - m_d[0];
      if (del > 40 && del < 300) {
        mov_byte = 1;
      }
      m_d[1] = m_d[0];
    }
    else {
      m_d[0] = 1000;
      m_d[1] = 1000;
      mov_byte = 0;
    }

    if (sign_check) {
      VU_check = true;
      VU();
      if (VU_sig) {
        vu_byte = 1;
      }
    }
    else {
      VU_check = false;
      VU_sig = false;
      if (!VU_sig) {
        vu_byte = 0;
      }
    }

    if (sign_check) {
      sign_byte = 8;
    }
    else {
      sign_byte = 9;
    }

    if (light_sen > 15) {
      light_byte = 1;
    }
    else {
      light_byte = 0;
    }

    mySerial.println("$" + String(t) + " " + String(h) + " " + String(P) + " " + String(distance) + " " + String(chance) + " " +
                     String(sign_byte) + " " + String(vu_byte) + " " + String(light_byte) + " " + String(mov_byte) + ";");
  }
}
