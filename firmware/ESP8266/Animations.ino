void mainLoop() {
  if (ONstate) {
    if (millis() - main_timer > MAIN_LOOP) {

      VU();

      animation();
      FastLED.show();         // отправить значения на ленту

      switch (this_mode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: FastLED.clear();
          break;
        case 6: if (ani_mode == 2 || ani_mode == 4)FastLED.clear();
          break;
      }
      main_timer = millis();    // сбросить таймер
    }
  }
}

void animation() {
  if (this_mode == 6) {
    if (auto_play_on_off == true) NextAutoMode();
  }
  switch (this_mode) {
    case 1:
      switch (temp_mode) {
        case 1: temperature();
          break;
        case 2: temperature_2();
          break;
      }
      break;

    case 2:
      switch (hum_mode) {
        case 1: humidity();
          break;
        case 2:  humidity_2();
          break;
      }

      break;

    case 3: 
    switch(bar_mode){
      case 1: pressure();
        break;
      case 2: pressure_2();
        break;  
    }
      break;

    case 4: Distance();
      break;

    case 5:
      switch (ColorMusic) {
        case 1: VU_meter_gyr();
          break;
        case 2: VU_meter_rainbow();
          break;
      }
      break;

    case 6: fullAnimation();
      break;

    case 7: colors();
      break;
  }
}



void NextAutoMode() {
  if (millis() - auto_timer > 60000) {
    ani_mode++;
    FastLED.clear();
    if (ani_mode > 18) ani_mode = 1;
    auto_timer = millis();
  }
}

void colors() {
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(r, g, b);
}






//*************************************** ТЕМПЕРАТУРА ***********************************
void temperature() {
  int lim_t = map(t, 0, 40, 0, NUM_LEDS + 1);

  count = 0; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < lim_t; i++)
  {
    leds[i] = ColorFromPalette(myPal_t, (count * ind));
    count++;
  }
}

void temperature_2() {
  int lim_t = map(t, 0, 40, 0, NUM_LEDS + 1);

  count = lim_t - 1; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(myPal_t, (count * ind));
  }
}



//*************************************** ВЛАЖНОСТЬ ***********************************
void humidity() {
  int lim_h = map(h, 0, 100, 0, NUM_LEDS + 1);
  count = 0; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < lim_h; i++)
  {
    leds[i] = ColorFromPalette(myPal_h, (count * ind));
    count++;
  }
}

void humidity_2() {
  int lim_h = map(h, 0, 100, 0, NUM_LEDS + 1);

  count = lim_h - 1; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(myPal_h, (count * ind));
  }
}

//*************************************** дИСТАНЦИЯ ***********************************

void Distance() { 
  dis_f = map(distance, 0, 250, 0, NUM_LEDS + 1) * SMOOTH_2 + dis_f * (1 - SMOOTH_2);

  count = 0;
  for (int i = 0; i < dis_f; i++)
  {
    leds[i] = ColorFromPalette(myPal_d, (count * ind));
    count++;
  }
}
//*************************************** ДАВЛЕНИЕ ***********************************

void pressure() {
  int lim_p = map(P, 730, 770, 0, NUM_LEDS + 1);
  count = 0; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < lim_p; i++)
  {
    leds[i] = ColorFromPalette(myPal_p, (count * ind));
    count++;
  }
}

void pressure_2() {
  int lim_p = map(P, 730, 770, 0, NUM_LEDS + 1);

  count = lim_p - 1; //Слева на право: С Начала ленты до конца
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(myPal_p, (count * ind));
  }
}

//--------------------------------------------------------ЭФФЕКТЫ (6)-------------------------------------------------------------------
void fullAnimation() {
  switch (ani_mode) {
    case 1: rainbow();
      break;

    case 2: lighter();
      break;

    case 3: damping();
      break;

    case 4: lighters_2();
      break;

    case 5: gPal = gPal1; fire();
      break;

    case 6: lightBugs_mode();
      break;

    case 7: increase();
      break;

    case 8: gPal = gPal2; fire();
      break;

    case 9: random_diods();
      break;

    case 10: gPal = gPal3; fire();
      break;

    case 11: sparkles();
      break;

    case 12: gPal = gPal4; fire();
      break;

    case 13: one_random();
      break;

    case 14: auto_colors();
      break;

    case 15: rainbow_up();
      break;

    case 16: hard_color();
      break;

    case 17: white();
      break;

    case 18: one_full_random();
      break;
  }
}


void fire() {
  if (effectTimer.isReady()) {
    random16_add_entropy(random(30000));
    Fire2012WithPalette();
  }
}


void Fire2012WithPalette()
{
  static byte heat[NUM_LEDS];
  for ( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }
  for ( int j = 0; j < NUM_LEDS; j++) {
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void auto_colors() {
  if (millis() - color_timer > COLOR_SPEED) {
    color_timer = millis();
    if (++LIGHT_COLOR > 255) LIGHT_COLOR = 0;
  }
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(LIGHT_COLOR, 255, 255);
}

void rainbow() {
  if (millis() - rainbow_timer > 30)
  {
    rainbow_timer = millis();
    count++;
  }
  for (int i = 0; i < NUM_LEDS; i++ )
  { // от 0 до первой трети
    leds[i] = CHSV(count + i * 10, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
}

void damping() {

  if (go_damp == false) {
    if (millis() - damping_timer > DAMPING_DELAY) {
      damping_timer = millis();
      go_damp = true;
    }
  }


  if (go_damp == true) {
    if (millis() - damping_timer_speed > DAMPING_SPEED) {
      damping_timer_speed = millis();
      damp_sped_flag = true;
    }

    if (damp_sped_flag == true) {
      if (brig_flag == false) {
        brig++;
        damp_sped_flag = false;
        if (brig == 255) {
          brig_flag = true;
          go_damp = false;
        }
      }
      if (brig_flag == true) {
        brig--;
        damp_sped_flag = false;
        if (brig == 0) {
          brig_flag = false;
          color = random(255);
          go_damp = false;
        }
      }
    }
    for (int i = 0 ; i < NUM_LEDS; i++ ) {
      leds[i] = CHSV(color, 255, brig);
    }

  }
}

void increase() {
  if (go_incr == false) {
    if (millis() - delay_timer_incr > INCREASE_DELAY)
    {
      delay_timer_incr = millis();
      go_incr = true;
      color = random(255);
      satur = random(170, 255);
      if (increase_ix == 0)increase_ix = -1;
      if (increase_ix == NUM_LEDS - 1) increase_ix++;
    }
  }

  if (go_incr == true) {
    if (millis() - increase_timer > INCREASE_SPEED) {
      increase_timer = millis();
      if (bound_incr == false) {
        increase_ix++;
        if (increase_ix == NUM_LEDS) {
          bound_incr = true;
          go_incr = false;

        }
      }
      if (bound_incr == true) {
        increase_ix--;
        if (increase_ix == 0) {
          bound_incr = false;
          go_incr = false;
        }
      }
      leds[increase_ix] = CHSV(color, satur, 255);
    }

  }
}

void lighter() {
  if (millis() - lighter_timer > LIGHTER_SPEED) {
    lighter_timer = millis();
    if (bound == false) {
      ix++;
      if (ix == NUM_LEDS) {
        bound = true;
        ix--;
      }
    }
    if (bound == true) {
      ix--;
      if (ix == 0) {
        bound = false;
        color = random(255);
        satur = random(170, 255);
      }
    }
  }
  leds[ix] = CHSV(color, satur, 255);
}

void lighters_2() {
  if (millis() - lighter_timer > LIGHTER_2_SPEED) {
    lighter_timer = millis();
    if (boun == false) {
      idexx++;
      if (idexx == NUM_LEDS) {
        boun = true;
        satur = random(100, 255);
        color = random(255);
        idexx--;
      }
    }
    if (boun == true) {
      idexx--;
      if (idexx == 0) {
        boun = false;
        satur = random(100, 255);
        color = random(255);
      }
    }

    if (bouncedirection == true) {
      idex--;
      if (idex == 0) {
        bouncedirection = false;
        satur = random(100, 255);
        col1 = random(255);
        idex--;
      }
    }
    if (bouncedirection == false) {
      idex++;
      if (idex == NUM_LEDS - 1) {
        bouncedirection = true;
        satur = random(100, 255);
        col1 = random(255);
      }
    }
  }
  leds[idex] = CHSV(col1, satur, 255);
  leds[idexx] = CHSV(color, satur, 255);
}

void lightBugs() {
  if (effectTimer.isReady()) {
    if (loadingFlag) {
      loadingFlag = false;
      for (byte i = 0; i < BUGS_AMOUNT; i++) {
        bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
        pos[i] = random(0, NUM_LEDS);
        speed[i] += random(-5, 6);
      }
    }
    if (millis() - bugs_timer > 10000) {
      bugs_timer = millis();
      for (byte i = 0; i < BUGS_AMOUNT; i++) {
        bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      }
    }
    FastLED.clear();
    for (byte i = 0; i < BUGS_AMOUNT; i++) {
      speed[i] += random(-5, 6);
      if (speed[i] == 0) speed[i] += (-5, 6);

      if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
      pos[i] += speed[i] / 10;
      if (pos[i] < 0) {
        pos[i] = 0;
        speed[i] = -speed[i];
      }
      if (pos[i] > NUM_LEDS - 1) {
        pos[i] = NUM_LEDS - 1;
        speed[i] = -speed[i];
      }
      leds[pos[i]] = bugColors[i];
    }
  }
}

void sparkles() {
  if (effectTimer.isReady()) {
    byte thisNum = random(0, NUM_LEDS);
    if (getPixColor(thisNum) == 0)
      leds[thisNum] = CHSV(random(0, 255), 255, 255);
    fade();
  }
}

void one_random() {
  if (go_one == false) {
    if (millis() - rand_diod_timer > ONE_RAND_DELAY) {
      rand_diod_timer = millis();
      go_one = true;
    }
  }

  if (go_one == true) {

    if (brightness_flag == false) {
      brig++;
      delay(3);
      if (brig == 255) {
        brightness_flag = true;
        go_one = false;
      }
    }
    if (brightness_flag == true) {
      brig--;
      delay(3);
      if (brig == 0) {
        brightness_flag = false;
        brig = 0;
        color = random(255);
        satur = random(170, 255);
        num = random(NUM_LEDS - 1);
        go_one = false;
        FastLED.clear();
      }
    }
  }
  leds[num] = CHSV(color, satur, brig);
}

void random_diods() {
  brig++;
  delay(7);
  if (brig == 255) {
    num = random(0, NUM_LEDS - 1);
    color = random(255);
    satur = random(170, 255);
    kol++;
    brig = 0;
  }
  if (kol == KOL_DIODS) {
    FastLED.clear();
    kol = 0;
  }
  leds[num] = CHSV(color, satur, brig);
}


void rainbow_up() {
  if (millis() - rainbow_timer > 30)
  {
    rainbow_timer = millis();
    count--;
  }
  for (int i = 0; i < NUM_LEDS; i++ )
  { // от 0 до первой трети
    leds[i] = CHSV(count + i * 10, 255, 255);  // HSV. Увеличивать HUE (цвет)
    // умножение i уменьшает шаг радуги
  }
}

void hard_color() {
  if (millis() - hcolor_timer > 10000) {
    hcolor_timer = millis();
    HARD_COLOR = random(0, 255);
  }
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(HARD_COLOR, 255, 255);
}


void white() {
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(0, 0, 255);
}

void one_full_random()
{
  if (id_m == NUM_LEDS)
  {
    ofr = false;
    if (millis() - one_fr_dl_timer > (del_ofr + ((MAIN_LOOP + 1) * 255 * NUM_LEDS)))
    {
      color = random(0, 255);
      id_m = 0;
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CHSV(0, 0, 0);
        FastLED.show();
        delay(70);
        m_rand[i] = -1;
      }
      ofr = true;
      one_fr_dl_timer = millis();
    }
  }

  if (ofr == true)
  {
    if (ofr_dl == true)
    {
      num = random(0, NUM_LEDS);
      for (int i = 0; i < NUM_LEDS; i++)
      {
        if (num == m_rand[i]) return;
      }
    }

    if (millis() - ofr_timer > 1)
    {
      leds[num] = CHSV(color, 255, ofr_br);
      ofr_br++;
      ofr_dl = false;
      if (ofr_br == 255) {
        ofr_dl = true;
        m_rand[id_m] = num;
        id_m++;
        ofr_br = 0;
      }
      ofr_timer = millis();
    }
  }
}

void lightBugs_mode() {
  if (effectTimer.isReady()) {
    if (loadingFlag) {
      loadingFlag = false;
      for (byte i = 0; i < BUGS_AMOUNT; i++) {
        bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
        pos[i] = random(0, NUM_LEDS);
        speed[i] += random(-5, 6);
      }
    }



    for (byte i = 0; i < BUGS_AMOUNT; i++) {
      if (random(5) == 1)speed[i] += random(-10, 10);
      if (speed[i] == 0) speed[i] += random(-1, 1) * 11;
      if (random(200) == 3) bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      if (abs(speed[i]) > MAX_SPEED) speed[i] = random(-5, 6);
      pos[i] += speed[i] / 10;
      if (pos[i] < 0) {
        pos[i] = 0;
        speed[i] = -speed[i] / 1.5;
      }
      if (pos[i] > NUM_LEDS - 1) {
        pos[i] = NUM_LEDS - 1;
        speed[i] = -speed[i] / 1.5;
      }

      leds[pos[i]].r = bugColors[i].r;
      leds[pos[i]].g = bugColors[i].g;
      leds[pos[i]].b = bugColors[i].b;
    }

    for (byte i = 0; i < NUM_LEDS; i++) {
      if (i != pos[i]) leds[i] = CRGB(constrain(leds[i].r - 40, 0, 255), constrain(leds[i].g - 40, 0, 255), constrain(leds[i].b - 40, 0, 255) );
    }
  }
}

#define TRACK_STEP 50
void fade() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) continue;
    leds[i].fadeToBlackBy(TRACK_STEP);
  }
}
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}
