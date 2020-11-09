void touch_v() {
  
  touch.tick();
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1:
        STAR();
        tempBrightness = BRIGHTNESS;
        break;
      case 2: UP();
        break;
      case 3: HASH();
        break;
      case 4: auto_play_on_off = !auto_play_on_off;
        break;
      case 5: fullLowPass();
        break;
    }
  }


  if (touch.isStep()) {
   
      wasStep = true;
      if (brightDirection) {
        BRIGHTNESS += 2;
      } else {
        BRIGHTNESS -= 2;
      }
      BRIGHTNESS = constrain(BRIGHTNESS, MIN_BRIGHTNESS, 255);
      FastLED.setBrightness(BRIGHTNESS);
      FastLED.show();
  }

  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }
}
