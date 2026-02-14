

//unsigned long int wifiStartTime;
//const unsigned long WIFI_TIMEOUT = 10 * 60 * 1000; // timeout de 10 min (o wifi)

//const char* ssid = "Sensor-BeeCounter";
//const char* password = "12345678"; // mínimo 8 caracteres

//#define ssid "JRTELECOMADOLFO" // Your WiFi SSID
//#define password "38632391"    // Your WiFi Password




/*
void startWifi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("\nIP fixo do Esp32: ");
  Serial.println(WiFi.softAPIP());
}
*/


/*


void WifiLoop() {
  if (digitalRead(button_WakeUp) == HIGH) {
    if (!wifiLigado) {
      Serial.println("Ligando WiFi AP...");
      startWifi();
      //setupServer();
      wifiLigado = true;
    } else {
      Serial.println("Desligando WiFi...");
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_OFF);
      wifiLigado = false;
    }
  }

  /*if (wifiLigado) {
    server.handleClient();
  }


  if (wifiLigado && millis() - wifiStartTime > WIFI_TIMEOUT) {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiLigado = false;
  }
}
*/



