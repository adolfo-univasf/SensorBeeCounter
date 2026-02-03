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
  Heltec.display->drawString(0, 18, "Tentando conectar a rede:");
  Heltec.display->drawString(0, 32, String(SSID));
  Heltec.display->drawString(0, 50, "Tentativas:");    
  Heltec.display->drawString(70, 50, String(cont));
  Heltec.display->display();
  delay(5000);
  ms_paraSair();
}

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
void display_Wifi_OK()
{
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(30, 25, "Wifi OK!");
  Heltec.display->display();
  delay(2000);
}

void ms_paraSair()
{
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "Md Configuração ativado");
  Heltec.display->drawString(0, 15, "para sair desligue");
  Heltec.display->drawString(0, 30, "a chave de configuração");
  Heltec.display->drawString(0, 45, "e reset a placa !");
  Heltec.display->display();
  delay(8000);
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








