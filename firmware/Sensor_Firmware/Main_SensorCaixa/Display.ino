void displaySetup()
{
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "Com Sucesso!");
  Heltec.display->display();
}

void display_Enviando_Dados()
{
  Heltec.display->displayOn();
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 17, "Enviando os ");
  Heltec.display->drawString(0, 33,  "dados ... ");
  Heltec.display->display();
}

void display_Dados()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
 // Heltec.display->drawString(0, 0, "Massa  : " + (String)currentMassa + " Kg ");
  //Heltec.display->drawString(0, 17, "Umida  : " + (String)currentHumidity + " %");
  //Heltec.display->drawString(0, 33,  "Tempe : " + (String)currentTemp + " °C");
  Heltec.display->drawString(0, 49, "Bateria : " + (String)currentBateria + " V ");
  Heltec.display->display();
}

void display_Modo_Configuracao()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 5, "Modo ");
  Heltec.display->drawString(0, 30, "Configuração!");
  Heltec.display->display();
  delay(1000);
}

void display_WiFi_Failed(int cont)
{
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(20, 0, "ERRO  WIFI ");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 18, "Falha ao configurar IP fixo!");
  Heltec.display->drawString(0, 50, "Tentativas:");    
  Heltec.display->drawString(70, 50, String(cont));
  Heltec.display->display();
  delay(5000);
}
/*
void display_IPServidor()
{
  ms_paraSair();
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 0, "Md Configuração");
  Heltec.display->drawString(0, 20, "IP Address: ");
  Heltec.display->drawString(0, 40, ip_server);
  Heltec.display->display();
  delay(6000);
}
*/
void display_Wifi_OK()
{
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);

  Heltec.display->drawString(0, 0, "WIFI Ligado!");
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "WIFI e Senha: ");
  Heltec.display->drawString(15, 20, "BeeCounter");
  Heltec.display->drawString(15, 40, " 12345678 ");
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Acesse o site :");
  Heltec.display->drawString(10, 30, "192.168.4.1");
  Heltec.display->display();
  delay(1000);
}

void displayDesligandoWifi()
{
  Heltec.display->displayOn();
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Tempo expirado! ");
  Heltec.display->drawString(0, 30, "Desligando WiFi...");
  Heltec.display->display();
  delay(3000);
  Heltec.display->displayOff();
}

void displayReadingsOnOled()
{
  //String temperatureDisplay ="Temperature: " + (String)currentTemp +  "°C";
 // String humidityDisplay = "Humidity: " + (String)currentHumidity + "%";
  Heltec.display->clear(); // Clear the OLED screen
  //Heltec.display->drawString(0, 0, temperatureDisplay); // Prepare to display temperature
  //Heltec.display->drawString(0, 12, humidityDisplay); // Prepare to display humidity
  Heltec.display->display(); // Display the readings
}








