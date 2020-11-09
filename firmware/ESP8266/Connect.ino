unsigned long sen_timer;

unsigned long hours;
int hours_col = 0;




void refreshData() {

  if (millis() - hours > 3600000) {
    hours_col++;
    hours = millis();
  }

  if (millis() - timer > 1000) {

    //----СИГНАЛИЗАЦИЯ
    if (sign_check) {
      m_d[0] = distance;
      int del = m_d[1] - m_d[0];
      if (del > 40 && del < 300) {
        mov_byte = 1;
        client.publish("Mov", String(mov_byte));
      }
      m_d[1] = m_d[0];
    }
    else {
      m_d[0] = 1000;
      m_d[1] = 1000;
      mov_byte = 0;
      client.publish("Mov", String(mov_byte));
    }

    if (sign_check) {
      VU_check = true;
      VU();
      if (VU_sig) {
        vu_byte = 1;
        client.publish("VU", String(vu_byte));
      }
    }
    else {
      VU_check = false;
      VU_sig = false;
      if (!VU_sig) {
        vu_byte = 0;
        client.publish("VU", String(vu_byte));
      }
    }

    if (sign_check) {
      sign_byte = 8;
      client.publish("Sign", String(sign_byte));
    }
    else {
      sign_byte = 9;
      client.publish("Sign", String(sign_byte));
    }

    if (light_sen > 10) {
      light_byte = 1;
      client.publish("Light", String(light_byte));
    }
    else {
      light_byte = 0;
      client.publish("Light", String(light_byte));
    }

    client.publish("Dis", String(distance));

    timer = millis();
  }

  if (millis() - sen_timer > 10000) {
    client.publish("Temp", String(t));
    client.publish("Hum", String(h));
    client.publish("Bar", String(P));
    client.publish("Chance", String(chance));

    client.publish("Hours", String(hours_col));

    sen_timer = millis();
  }
}

void MQTT_connect() {
  if (mqtt_run) {
    if (!client.connected())
    {
      //client.set_server(mqtt_server, mqtt_port);
      Serial.println("Con");
      if (client.connect(MQTT::Connect("Table_Lamp").set_auth(mqtt_user, mqtt_pass)))
      {
        Serial.println("+");
        client.set_callback(callback);
        // подписываемся под топики
        client.subscribe("Hours");
        client.subscribe("Temp");
        client.subscribe("Hum");
        client.subscribe("Dis");
        client.subscribe("Bar");
        client.subscribe("Chance");

        client.subscribe("color");
        client.subscribe("move");
        client.subscribe("brig");


        client.subscribe("Mov");
        client.subscribe("Light");
        client.subscribe("VU");
        client.subscribe("Sign");
      }
      else {
        Serial.println(":(");
      }
    }

    if (client.connected())
    {
      client.loop();
      refreshData();
    }
  }
}
