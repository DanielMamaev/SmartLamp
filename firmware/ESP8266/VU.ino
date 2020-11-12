// ----- сигнал
#define EXP 1.4                   // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)
byte EMPTY_BRIGHT = 30;           // яркость "не горящих" светодиодов (0 - 255)
// ----- режим шкала громкости
float SMOOTH = 0.22;              // коэффициент плавности анимации VU (по умолчанию 0.5)
#define MAX_COEF 1.8              // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)
float RAINBOW_STEP = 5.5;
byte volume;
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
float RsoundLevel,RsoundLevel_f;
int RcurrentLevel;
float averageLevel = 50;
float averK = 0.006;
int maxLevel = 100;
byte Rlenght;
int hue;


void VU() {
  RsoundLevel = 0;
  volume = 0;
  for (byte i = 0; i < 100; i ++) {                                 // делаем 100 измерений
    RcurrentLevel = analogRead(SOUND_R);                            // с правого

    if (RsoundLevel < RcurrentLevel) RsoundLevel = RcurrentLevel;   // ищем максимальное
  }

  // фильтруем по нижнему порогу шумов
  RsoundLevel = map(RsoundLevel, LOW_PASS, 1023, 0, 500);

  // ограничиваем диапазон
  RsoundLevel = constrain(RsoundLevel, 0, 500);

  // возводим в степень (для большей чёткости работы)
  RsoundLevel = pow(RsoundLevel, EXP);

  // фильтр
  RsoundLevel_f = RsoundLevel * SMOOTH + RsoundLevel_f * (1 - SMOOTH);

  // если значение выше порога - начинаем самое интересное
  if (RsoundLevel_f > 15) {

    // расчёт общей средней громкости с обоих каналов, фильтрация.
    // Фильтр очень медленный, сделано специально для автогромкости
    averageLevel = (float)RsoundLevel_f * averK + averageLevel * (1 - averK);

    // принимаем максимальную громкость шкалы как среднюю, умноженную на некоторый коэффициент MAX_COEF
    maxLevel = (float)averageLevel * MAX_COEF;

    // преобразуем сигнал в длину ленты (где MAX_CH это половина количества светодиодов)
    Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, NUM_LEDS);


    // ограничиваем до макс. числа светодиодов
    Rlenght = constrain(Rlenght, 0, NUM_LEDS);
    

    if (VU_check) {
      volume = (100 * Rlenght) / NUM_LEDS;
      if (volume > 18) {
        VU_sig = true;
      }
    }
  }
}

void VU_meter_gyr() {
  count = 0; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < Rlenght; i++) {
    leds[i] = ColorFromPalette(myPal, (count*ind));   // заливка по палитре " от зелёного к красному"
    count++;
  }
}

void VU_meter_rainbow() {
  if (millis() - rainbow_timer > 100) {
    rainbow_timer = millis();
    hue = floor((float)hue - RAINBOW_STEP);
  }
  count = 0; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < (0 + Rlenght); i++ ) {
    leds[i] = ColorFromPalette(RainbowColors_p, (count * ind) / 2 - hue); // заливка по палитре радуга
    count++;
  }
}
void autoLowPass() {
  // для режима VU
  delay(10);                                // ждём инициализации АЦП
  int thisMax = 0;                          // максимум
  int thisLevel;
  for (byte i = 0; i < 200; i++) {
    thisLevel = analogRead(SOUND_R);        // делаем 200 измерений
    if (thisLevel > thisMax)                // ищем максимумы
      thisMax = thisLevel;                  // запоминаем
    delay(4);                               // ждём 4мс
  }
  LOW_PASS = thisMax + LOW_PASS_ADD;        // нижний порог как максимум тишины + некая величина
}

void fullLowPass() {
  FastLED.setBrightness(0); // погасить ленту
  FastLED.clear();          // очистить массив пикселей
  FastLED.show();           // отправить значения на ленту
  delay(500);               // подождать чутка
  autoLowPass();            // измерить шумы
  delay(500);               // подождать
  FastLED.setBrightness(BRIGHTNESS);  // вернуть яркость
}
