#define HIST_MAX 10

struct EstadoLog {
  uint8_t estado;
  unsigned long tempo;
};

EstadoLog hist[HIST_MAX];
uint8_t histLen = 0;


void gravaBuffer(uint8_t estado) {
  if (histLen == 0 || hist[histLen - 1].estado != estado) {
    if (histLen < HIST_MAX) {
      hist[histLen].estado = estado;
      hist[histLen].tempo = millis();
      histLen++;
    }
  }
}

// FSM (Finite State Machine)

void processaFSM(uint8_t estado) {

  unsigned long agora = millis();

  // timeout geral
  if (agora - tempoEstado > TIMEOUT_FSM)
  {
    if( estado == STATE_2   &&  (Quant_timeout_state2 > 0) )
    {  // Quant_timeout_state2 é a quantidade de vezes o timeout que foi acrescentado por parar no estado2, 
      tempoEstado = agora;
      Quant_timeout_state2 --;
      Serial.println("Quant_timeout_state2: ");
      Serial.print(Quant_timeout_state2);
      Serial.println("");
    }
    else
    { // timeout padrão
      Quant_timeout_state2 = 2;
      estadoFSM = FSM_IDLE;
      tempoEstado = agora;
      Serial.println("[FSM] TIMEOUT -> FSM_IDLE");
      return;
    }


  }

  switch (estadoFSM) {

    // ===============================
    case FSM_IDLE:
      if (estado == STATE_1) {
        estadoFSM = FSM_S1_SEEN;
        sentidoFSM = ENTRADA;
        tempoEstado = agora;
        Serial.println("[FSM] FSM_IDLE -> STATE_1");
      }
      else if (estado == STATE_3) {
        estadoFSM = FSM_S3_SEEN;
        sentidoFSM = SAIDA;
        tempoEstado = agora;
        Serial.println("[FSM] FSM_IDLE -> STATE_3");
      }
      break;

    // ===============================
    case FSM_S1_SEEN:
      if (estado == STATE_2) {
        estadoFSM = FSM_S1_S2_SEEN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_1 -> STATE_2");
      }
      break;

    case FSM_S3_SEEN:
      if (estado == STATE_2) {

        estadoFSM = FSM_S3_S2_SEEN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_3 -> STATE_2");
      }
      break;

    // ===============================
    case FSM_S1_S2_SEEN:
      if (estado == STATE_3) {
        estadoFSM = FSM_ALL_SEEN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_1 -> STATE_2 -> STATE_3");
      }else if(estado == STATE_1){
        estadoFSM = FSM_RETURN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_1 -> STATE_2 -> STATE_1");
      }
      break;

    case FSM_S3_S2_SEEN:
      if (estado == STATE_1) {
        estadoFSM = FSM_ALL_SEEN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_3 -> STATE_2 -> STATE_1");
      }else if(estado == STATE_3){
        estadoFSM = FSM_RETURN;
        tempoEstado = agora;
        Serial.println("[FSM] STATE_3 -> STATE_2 -> STATE_3");
      }
      break;

    // ===============================
    case FSM_ALL_SEEN:
      if (estado == WAIT) {
        if (sentidoFSM == ENTRADA) {
          Serial.println("[FSM] STATE_1 -> STATE_2 -> STATE_3 ->");
          registraEvento(estadoFSM);
        } else {
          Serial.println("[FSM] STATE_3 -> STATE_2 -> STATE_1 ->");
          registraEvento(estadoFSM);
        }
        estadoFSM = FSM_IDLE;
        tempoEstado = agora;
      }
      break;




      case FSM_RETURN:
      if (estado == WAIT) {
        if (sentidoFSM == ENTRADA) {
          Serial.println("[FSM] STATE_1 -> STATE_2 -> STATE_1 ->");
          registraEvento(estadoFSM);
        } else {
          Serial.println("[FSM] STATE_3 -> STATE_2 -> STATE_3 ->");
          registraEvento(estadoFSM);
        }
        estadoFSM = FSM_IDLE;
        tempoEstado = agora;
      }
      break;

  }
}

void registraEvento(uint8_t ultimoEstadoFSM) {
  
  /*struct tm timeinfo;
  getLocalTime(&timeinfo);

  char data[11];
  char hora[9];

  strftime(data, sizeof(data), "%d/%m/%Y", &timeinfo);
  strftime(hora, sizeof(hora), "%H:%M:%S", &timeinfo);

  File f = SPIFFS.open("/historico.csv", "a");
  */
  if(ultimoEstadoFSM == FSM_RETURN){

    if (sentidoFSM == ENTRADA) {
    Serial.println("");
    Serial.print(" RETORNO PARA O CAMPO REGISTRADA AS ");
    Serial.print(ReadtimeRTC('F'));
    Serial.println("\n");
    ReturnFieldBuffer++; // retorna para o campo

    //f.printf("%s,%s,ENTRADA\n", data, hora);
  } else {
    Serial.println("");
    Serial.print("RETORNO PARA A COLMEIA REGISTRADA AS ");
    Serial.print(ReadtimeRTC('F'));
    Serial.println("\n");
    ReturnHiveBuffer++; // retorna para a colmeia
    //f.printf("%s,%s,SAIDA\n", data, hora);
  }


  }else if(ultimoEstadoFSM == FSM_ALL_SEEN){

    if (sentidoFSM == ENTRADA) {
    Serial.println("");
    Serial.print(" ENTRADA NA COLMEIA REGISTRADA AS ");
    Serial.print(ReadtimeRTC('F'));
    Serial.println("\n");
    EntradasBuffer++;

    //f.printf("%s,%s,ENTRADA\n", data, hora);
    } else {
    Serial.println("");
    Serial.print("SAIDA PARA O CAMPO REGISTRADA AS ");
    Serial.print(ReadtimeRTC('F'));
    Serial.println("\n");
    SaidasBuffer++;
    //f.printf("%s,%s,SAIDA\n", data, hora);
  }

  }

  

  //f.close();
}


void printEstadoSensores(uint8_t estado) {
  Serial.print("[SENSORES] ");

  switch (estado) {
    case WAIT:        Serial.println("WAIT (111)"); break;
    case STATE_3:     Serial.println("STATE_3 (110)"); break;
    case STATE_2:     Serial.println("STATE_2 (101)"); break;
    case STATE_2_3:   Serial.println("STATE_2_3 (100)"); break;
    case STATE_1:     Serial.println("STATE_1 (011)"); break;
    case STATE_1_3:   Serial.println("STATE_1_3 (010)"); break;
    case STATE_1_2:   Serial.println("STATE_1_2 (001)"); break;
    case STATE_1_2_3: Serial.println("STATE_1_2_3 (000)"); break;
    default:          Serial.println("DESCONHECIDO"); break;
  }
}

