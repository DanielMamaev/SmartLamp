#define PARSE_AMOUNT 8         // число значений в массиве, который хотим получить
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean recievedFlag;
boolean getStarted;
byte indexx;
String string_convert = "";
void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != ';') {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        intData[indexx] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        indexx++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$') {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      indexx = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}

void parsing_nano() {
  parsing();
  if (recievedFlag) {
    recievedFlag = false;
    /*for (byte i = 0; i < PARSE_AMOUNT; i++) {  // выводим элементы массива
      Serial.print(intData[i]); Serial.print(" ");
      }
      Serial.println();*/
    t = intData[0];
    h = intData[1];
    P = intData[2];
    distance = intData[3];
    light_sen = intData[4];
    chance = intData[7];


    if (intData[6] != 0) FastLED.setBrightness(intData[6]);

    indi_mode_bool = true;
    switch (intData[5]) {
      case 1: this_mode = 1;
        break;
      case 2: this_mode = 2;
        break;
      case 3: this_mode = 3;
        break;
      case 4: this_mode = 4;
        break;
      case 5: this_mode = 5;
        break;
      case 6: this_mode = 6;
        break;
      case 7: this_mode = 7;
        break;
      case 9:
        intData[5] = 0;
        res_wifi();

        //check_mqtt();
        break;
      case 10: auto_play_on_off = !auto_play_on_off;
        break;

      case 17: fullLowPass();
        break;
      case 11: STAR();
        break;
      case 12: HASH();
        break;
      case 15: LEFT();
        break;
      case 16: RIGHT();
        break;
    }
    intData[5] = 0;
  }
}

/*void EEPROM_int_write(int addr, int num) {
  byte raw[2];
  (int&)raw = num;
  for (byte i = 0; i < 2; i++) EEPROM.write(addr + i, raw[i]);
  }

  int EEPROM_int_read(int addr) {
  byte raw[2];
  for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
  int &num = (int&)raw;
  return num;
  }*/
