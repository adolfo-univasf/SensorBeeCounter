




void sendPacket()   //função de envio quando a interrupção é chamada
{
  LoRa.beginPacket();
  LoRa.print(IDcaixa);
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


/*
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
*/