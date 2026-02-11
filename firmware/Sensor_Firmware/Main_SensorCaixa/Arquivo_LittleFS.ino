//  You only need to format LittleFS the first time you run a
//  test or else use the LITTLEFS plugin to create a partition 
//  https://github.com/lorol/arduino-esp32littlefs-plugin

#define FORMAT_LITTLEFS_IF_FAILED true

void BeginLittleFS(){

  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
    Serial.println("LittleFS Mount Failed");
    return;
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");

            (file.size() < 1025)? Serial.print(file.size()) : Serial.print(file.size()/1024); // for menor que 1024 B imprime como bytes, se for maior imprime como Kbytes
            (file.size() < 1025)? Serial.print(" B\n") : Serial.print(" KB\n");               // for menor que 1024 B imprime como bytes, se for maior imprime como Kbytes
        }
        file = root.openNextFile();
    }
}


float sizeFile(fs::FS &fs, const char * path){

  File file = fs.open(path);
  float FileSize = file.size();

  Serial.print("  FILE: ");
  Serial.print(file.name());
  Serial.print("\tSIZE: ");
  (file.size() < 1025)? Serial.print(file.size()) : Serial.print(file.size()/1024); // for menor que 1024 B imprime como bytes, se for maior imprime como Kbytes
  (file.size() < 1025)? Serial.print(" B\n") : Serial.print(" KB\n");               // for menor que 1024 B imprime como bytes, se for maior imprime como Kbytes]
  
  file.close();
  return (FileSize/1024.0);
}




void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    

    
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}





bool FileExiste(fs::FS &fs, const char * path){
  File file = fs.open(path);
  if(!file){
      Serial.println("\nO arquivo ainda não existe!");
      return false;
  }
  Serial.println("\nO arquivo existe!");
  file.close();
  return true;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    // caso o arquivo esteja vazio escreve o cabeçalho
    if (file.size() == 0) {
      file.println("Data,Entradas,Saidas,Bateria,Erros");
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();

    String data = "2026-01-24T14:35:00";
    uint16_t entradas = 123;
    uint16_t saidas = 118;
    float bateria = 12.8;
    uint8_t erros = 0;

   for(i=0; i<20000; i++){
    file.print(data);
    file.print(",");
    file.print(entradas);
    file.print(",");
    file.print(saidas);
    file.print(",");
    file.print(bateria, 1); // 1 casa decimal
    file.print(",");
    file.println(erros); // println = nova linha
  }

    /*
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    */
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}

volatile bool flagAtualizar = false;

const char PAGE_HTML[] PROGMEM = R"rawliteral(

  <!DOCTYPE html>
  <html lang="pt-BR">
  <head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sensor BeeCounter</title>

  <style>
  body {
  margin: 0;
  padding: 20px;
  font-family: Arial, sans-serif;
  background: #f4f4f4;
  text-align: left;
  }
  
  h1 {
  font-size: 2.2rem;
  margin-bottom: 30px;
  }
  
  h2 {
  margin-left: 5px;
  
  font-size: 1.4rem;
  margin-bottom: 15px;
  }
  
  div {
    margin-left: 10px;
  }


  table.dados {
    width: 90%;
    max-width: 400px;
    border-collapse: collapse;
    margin-left: 5px;
    margin-bottom: 20px;
    background: #fff;
    border-radius: 8px;
    overflow: hidden;
    box-shadow: 0 2px 5px rgba(0,0,0,0.1);
  }

  table.dados th {
    text-align: left;
    padding: 10px;
    background: #0066cc;
    color: white;
    font-weight: normal;
    width: 40%;
  }

  table.dados td {
    padding: 10px;
    border-bottom: 1px solid #ddd;
  }

  table.dados tr:last-child td {
    border-bottom: none;
  }

  .linha-data {
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .btn-sync {
    width: auto;
    padding: 5px 10px;
    margin: 0;
    font-size: 1.2rem;

    border-radius: 6px;
  }

  .file {
    margin-bottom: 10px;
  }

  button {
    margin-left: 5px;
    margin-top: 5px;
    margin-bottom: 5px;
    width: 100%;
    max-width: 250px;
    font-size: 1rem;
    padding: 10px;
    border: none;
    border-radius: 8px;
    background: #0066cc;
    color: #fff;
    cursor: pointer;
    box-sizing: border-box; /* evita que padding aumente o tamanho */
    transition: background 0.3s ease;
  }

  /* Container dos botões alinhados */
  .botoes-arquivo {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-top: 2px;
    /* Para os botões ocuparem só o espaço necessário dentro do flex */
  }

  /* Botão Download: usa base genérica, só restringe largura para não ocupar toda */
  .btn-download {
    width: auto;       /* sobrescreve width 100% */
    min-width: 120px;  /* largura mínima para ficar confortável */
    padding: 10px 16px; /* padding mais largo */
  }

  /* Botão Delete: mantém estilo parecido, mas menor e vermelho */
  .btn-delete {
    width: auto;
    min-width: 40px;      /* largura menor, só para o ícone */
    padding: 10px 12px;    /* padding menor */
    background: #cf3a3a;  /* vermelho */
    color: #fff;
    font-size: 1.2rem;    /* fonte um pouco maior para o ícone */
    display: flex;
    justify-content: center;
    align-items: center;
  }

  /* Hover para download e delete (overrides genérico) */
  .btn-download:hover {
    background: #004a99;
  }

  .btn-delete:hover {
    background: #9c1e1e;
  }
  
  footer {
  margin-left: 5px;
  margin-top: 50px;
  font-size: 0.85rem;
  color: #666;
  }



  </style>
  
  </head>

  <body>

  <h1>Sensor BeeCounter</h1>
  <h2>Dados Atuais:</h2>

  <table class="dados">
    <tr>
      <th>📆 Data</th>
      <td class="linha-data">
	  <span>%V_DATA%</span>
	  <button class="btn-sync" onclick="sincronizarDataHora()"> ⚙️ </button>
	  </td>
	  
    </tr>
  <tr>
    <th>🕑 Hora</th>
    <td>%V_HORA%</td>
  </tr>
  <tr>
    <th>➡️ Entradas</th>
    <td>%V_ENTRADAS%</td>
  </tr>
  <tr>
    <th>⬅️ Saídas</th>
    <td>%V_SAIDAS%</td>
  </tr>
  <tr>
    <th>🔋 Bateria (V)</th>
    <td>%V_BATERIA%</td>
  </tr>
    <tr>
    <th>⚠️ Erros</th>
    <td>%V_Erros%</td>
  </tr>
</table>

  <h2>Download arquivos:</h2>
  
  <div id="files">
  %V_FILE_LIST%
  </div>
  
  <footer>
  <b>Eng. Adolfo Castro</b>
  </footer>

  <script>

  function sincronizarDataHora() 
  {
    const agora = new Date();
    const dados = {
      ano: agora.getFullYear(),
      mes: agora.getMonth() + 1,
      dia: agora.getDate(),
      hora: agora.getHours(),
      minuto: agora.getMinutes(),
      segundo: agora.getSeconds()
    };
    // 🔹 Função para adicionar zero à esquerda
    const doisDigitos = (num) => num.toString().padStart(2, '0');

    const dataFormatada =
      `${doisDigitos(dados.dia)}/` +
      `${doisDigitos(dados.mes)}/` +
      `${dados.ano} ` +
      `${doisDigitos(dados.hora)}:` +
      `${doisDigitos(dados.minuto)}:` +
      `${doisDigitos(dados.segundo)}` ;


    if (!confirm(`Deseja atualizar a data e hora do dispositivo para: \n\n${dataFormatada}?`)) {
    return;
    }

    fetch("/setDateTime", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(dados)
    })
    .then(response => response.text())
    .then(data => {
      alert("Data/Hora atualizada com sucesso!");
    })
    .catch(err => {
      alert("Erro ao atualizar.");
    });
  }

  function confirmarExclusao(nomeArquivo) {

    if (!confirm(`Deseja realmente apagar o arquivo:\n\n${nomeArquivo} ?`)) {
      return;
    }

    fetch(`/delete?file=${nomeArquivo}`)
      .then(response => response.text())
      .then(data => {
        alert("Arquivo apagado com sucesso!");
        location.reload(); // Atualiza lista
      })
      .catch(err => {
        alert("Erro ao apagar arquivo.");
      });
  }


</script>



  </body>
  </html>
)rawliteral";

const char PAGE_FILE_NOT_FOUND[] PROGMEM = R"rawliteral(

  <!DOCTYPE html>
  <html lang="pt-BR">
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <h1 style='font-size:2rem;text-align:center'>Arquivo não encontrado!</h1>

)rawliteral";

// *****************************************************************************************************************


String processor(const String& var) {
  if (var == "V_DATA")    return String(ReadtimeRTC('D'));
  if (var == "V_HORA")    return String(ReadtimeRTC('H'));
  if (var == "V_BATERIA") return String(Bateria);
  if (var == "V_ENTRADAS") return String(EntradasBuffer);
  if (var == "V_SAIDAS")   return String(SaidasBuffer);
  if (var == "V_FILE_LIST")  return gerarHTMLArquivos();
  return "";
}

void UpdateWeb(){

  /*
  // Rota principal "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Envia direto da PROGMEM e processa os marcadores
    request->send_P(200, "text/html", PAGE_HTML, processor);
  });

  // Rota de download
  server.on("/download", HTTP_GET, handleFileDownload);

  // Inicia o servidor
  server.begin();


*/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
                  
    String page = PAGE_HTML; // copia do PROGMEM
    
    page.replace("%V_FILE_LIST%", gerarHTMLArquivos()); 
    //page.replace("<!-- FILE_LIST -->", gerarHTMLArquivos()); 
    page.replace("%V_DATA%", ReadtimeRTC('D'));
    page.replace("%V_HORA%", ReadtimeRTC('H'));
    page.replace("%V_BATERIA%", Bateria);
    page.replace("%V_ENTRADAS%", String(EntradasBuffer));
    page.replace("%V_SAIDAS%", String(SaidasBuffer));
    page.replace("%V_Erros%", String(Erros));
    
    request->send(200, "text/html", page);
  });
  //request->send_P(200, "text/html", PAGE_HTML, processor);

  server.on("/download", HTTP_GET, handleFileDownload);


  server.on("/setDateTime", HTTP_POST,[](AsyncWebServerRequest *request){}, NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      request->send(400, "text/plain", "JSON inválido");
      return;
    }
    int ano     = doc["ano"];
    int mes     = doc["mes"];
    int dia     = doc["dia"];
    int hora    = doc["hora"];
    int minuto  = doc["minuto"];
    int segundo = doc["segundo"];
    // ✅ Validação básica
    if (ano < 2024 || mes < 1 || mes > 12 ||
        dia < 1 || dia > 31 ||
        hora < 0 || hora > 23 ||
        minuto < 0 || minuto > 59 ||
        segundo < 0 || segundo > 59) {

      request->send(400, "text/plain", "Data/Hora inválida");
      return;
    }
    // Ajusta o RTC
    rtc.adjust(DateTime(ano, mes, dia, hora, minuto, segundo));
    request->send(200, "text/plain", "OK");
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){

    if (!request->hasParam("file")) {
      request->send(400, "text/plain", "Arquivo não informado");
      return;
    }

    String nomeArquivo = request->getParam("file")->value();

    // 🔒 Proteção contra Path Traversal
    if (nomeArquivo.indexOf("..") >= 0) {
      request->send(400, "text/plain", "Nome inválido");
      return;
    }

    // 🔒 garantir que começa com "/"
    if (!nomeArquivo.startsWith("/")) {
      nomeArquivo = "/" + nomeArquivo;
    }

    // 🔒 Permitir apenas .csv
    if (!nomeArquivo.endsWith(".csv")) {
      request->send(400, "text/plain", "Extensão não permitida");
      return;
    }
    // 🔎 Verifica se existe
    if (!LittleFS.exists(nomeArquivo)) {
      request->send(404, "text/plain", "Arquivo não encontrado");
      return;
    }

    if (LittleFS.remove(nomeArquivo)) {
      request->send(200, "text/plain", "OK");
    } else {
      request->send(500, "text/plain", "Erro ao remover");
    }
  });
  server.begin();
}

String formatarTamanho(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024.0, 2) + " KB";
  } else {
    return String(bytes / (1024.0 * 1024.0), 2) + " MB";
  }
}

String gerarHTMLArquivos() {
  //String html = "<!DOCTYPE html><html><body>";
  
  String html;

  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    html += "<p>Diretório nao encontrado.</p>";
    return html;
  }

  File file = root.openNextFile();

  Serial.println("\n****************************************************************");
  Serial.print("root: ");
  Serial.print(root);
  Serial.print("\nfile: ");
  Serial.print(file);
  Serial.println("");
  while (file) {
    if (!file.isDirectory()) {
      String nome = String(file.name());
      size_t tamanho = file.size();
      Serial.print("\nfile: ");
      Serial.print(file);
      Serial.print("\nnome: ");
      Serial.print(nome);
      nome.replace("/", "");
      
      html += "<div class='file'>";
      html += "<strong>" + nome +"</strong>  [";
      html += formatarTamanho(tamanho) + "]<br>";

      html += "<div class='botoes-arquivo'>";
      html += "<button onclick=\"window.location.href='/download?file=";
      html += nome;
      html += "'\">📥   Download</button>";
      
      // 🗑 Botão deletar
      html += "<button class='btn-delete' onclick=\"confirmarExclusao('";
      html += nome;
      html += "')\"> 🗑️ </button>";
      
      
      html += "</div>";
      html += "</div>";
    }
    file = root.openNextFile();
  }
   Serial.println("\n****************************************************************");

  //html += "</body></html>";
  return html;
}

void handleFileDownload(AsyncWebServerRequest *request) {

  if (!request->hasParam("file")) {
    request->send(400, "text/plain", "Arquivo não especificado");
    return;
  }

  String fileName = request->getParam("file")->value(); // ex: Dados.csv
  String filePath = "/" + fileName;

  if (!LittleFS.exists(filePath)) {
    request->send_P(404, "text/html", PAGE_FILE_NOT_FOUND);
    return;
  }

  AsyncWebServerResponse *response =
    request->beginResponse(
      LittleFS,
      filePath,
      "application/octet-stream",
      true   // true = download (Content-Disposition)
    );

  response->addHeader(
    "Content-Disposition",
    "attachment; filename=\"" + fileName + "\""
  );

  request->send(response);

}



