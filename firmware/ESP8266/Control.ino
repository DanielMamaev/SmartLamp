void STAR() {
  ONstate = !ONstate; FastLED.clear(); FastLED.show();
}

void RIGHT() {
  FastLED.clear();
  switch (this_mode) {
    case 1: if (++temp_mode > 2) temp_mode = 1;
      break;
    case 2: if (++hum_mode > 2) hum_mode = 1;
      break;
    case 3: if (++bar_mode > 2) bar_mode = 1;
      break;
    case 5: if (++ColorMusic > 2) ColorMusic = 1;
      break;
    case 6: if (++ani_mode > 18) ani_mode = 1;
      break;

  }

}
void LEFT() {
  FastLED.clear();
  switch (this_mode) {
    case 1: if (--temp_mode < 1) temp_mode = 2;
      break;
    case 2: if (--hum_mode < 1) hum_mode = 2;
      break;
    case 3: if (--bar_mode < 1) bar_mode = 2;
      break;
    case 5: if (--ColorMusic < 1) ColorMusic = 2;
      break;
    case 6: if (--ani_mode < 1) ani_mode = 18;
      break;
  }
}


void HASH() {
  //if (settings_mode) esp_reset = true;
  indi_mode_bool = true;
  switch (this_mode) {
    case 1:
      byte t_indi;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      t_indi = t / 10;
      for (byte i = 0; i < t_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      t_indi = t % 10;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      for (byte i = 0; i < t_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      break;
    case 2:
      byte h_indi;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      h_indi = h / 10;
      for (byte i = 0; i < h_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      h_indi = h % 10;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      for (byte i = 0; i < h_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      break;

    case 3:
      int p_indi;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      p_indi = P / 100;
      for (int i = 0; i < p_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);

      p_indi = P / 10;
      p_indi = p_indi % 10;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      for (int i = 0; i < p_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);

      p_indi = P % 100;
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      for (int i = 0; i < p_indi; i++) {
        digitalWrite(CLOCK, HIGH);
        digitalWrite(CLOCK, LOW);
      }
      delay(1500);
      digitalWrite(RESET, HIGH);
      digitalWrite(RESET, LOW);
      delay(1000);
      break;
  }
}


void UP() {
  indi_mode_bool = true;
  if (++this_mode > MODE_AMOUNT) this_mode = 1;
  FastLED.clear();
}

void DOWN()
{
  indi_mode_bool = true;
  if (--this_mode < 1) this_mode = MODE_AMOUNT;
  FastLED.clear();
}
