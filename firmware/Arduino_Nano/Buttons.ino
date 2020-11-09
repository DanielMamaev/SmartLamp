void remoteTick() {
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;
    switch (IRdata) {
      // режимы
      case BUTT_1: mov = 1;
        break;
      case BUTT_2: mov = 2;
        break;
      case BUTT_3: mov = 3;
        break;
      case BUTT_4: mov = 4;
        break;
      case BUTT_5: mov = 5;
        break;
      case BUTT_6: mov = 6;
        break;
      case BUTT_7: mov = 7;
        break;
      case BUTT_8:
        break;
      case BUTT_9: mov = 9;
        break; 
      case BUTT_0: mov = 10;
        break;
        
      case BUTT_STAR: mov = 11;
        break;
      case BUTT_HASH: mov = 12; 
        break;
      case BUTT_OK: mov = 17;
        break;
      case BUTT_UP: UP(); 
        break;
      case BUTT_DOWN: DOWN(); 
        break;
      case BUTT_LEFT: mov = 15;
        break;
      case BUTT_RIGHT: mov = 16; 
    }
  }
}


void DOWN() {
  BRIGHTNESS = smartIncr(BRIGHTNESS, -20, 0, 255);
  send_brig=BRIGHTNESS;
}
void UP() {
  BRIGHTNESS = smartIncr(BRIGHTNESS, 20, 0, 255);
  send_brig=BRIGHTNESS;
}
