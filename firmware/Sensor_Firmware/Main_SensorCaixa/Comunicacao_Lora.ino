
void sendPacket()   //função de envio quando a interrupção é chamada
{
  LoRa.beginPacket();
 // LoRa.print(IDcaixa);
  LoRa.print('T');
  //LoRa.print(currentTemp);
  LoRa.print('U');
 // LoRa.print(currentHumidity);
  LoRa.print('M');
 // LoRa.print(currentMassa);
  LoRa.print('B');
  LoRa.print(currentBateria);
  LoRa.endPacket();

}

void EscutaGateway(int packetSize){
  String Dados_Sensor = "",Resposta,dados,sensor = "";
  String DadosLora = "";
  packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  while (LoRa.available()) {
    DadosLora += (char)LoRa.read();
  }

  Serial.println("\nDadosLora:");
  Serial.print(DadosLora);

  /* 
  Padrão de Resposta deve ser 
      MEL ! Dados ! F
  */
  
  sensor = "MEL?";
  sensor += ID_Sensor;

 if (DadosLora == sensor)
 {
  String DadosEnviar = "", Separador = "#";
  DadosEnviar += "MEL";
  DadosEnviar += Separador;
  DadosEnviar += ID_Sensor;
  DadosEnviar += Separador;
  DadosEnviar += DadosAtuaisLora;
  DadosEnviar += Separador;
  DadosEnviar += "F";
  LoRa.beginPacket();
  LoRa.print(DadosEnviar);  // Envia os ultimos dados coletados para o gateway
  LoRa.endPacket();
  Serial.println("\n****************  Dados Enviados ao Gateway *************\n");
  Serial.println(DadosEnviar);
 }
}

String Concatena_Dados_Lora(int packetSize) {
  String DadosLora = "";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++)
  {  
    DadosLora += (char) LoRa.read(); //Atribui um caractere por vez a váriavel DadosLora
  }
  return DadosLora;
  //LoRaDataPrint();
}
