#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>



#include <WiFi.h>
#include <WebServer.h>

#define SSID "JRTELECOMADOLFO" // Your WiFi SSID
#define PASSWORD "38632391"    // Your WiFi Password

WebServer server(80);

extern const char PAGE_HTML[] PROGMEM; // HTML web page


void setup(){


    Serial.begin(115200);

    BeginLittleFS();

    Serial.println("\nConectando ao WiFi...");
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.print("IP local do ESP32: ");
    Serial.print(WiFi.localIP());
    Serial.println("\n");

    // Página principal
    server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", PAGE_HTML);
    });

    server.on("/download", HTTP_GET, []() 
    {
      //File file = LittleFS.open("/mydir/Dados.csv", "r");
      File file = LittleFS.open("/test.csv", "r");

      if (!file) {
        server.send(404, "text/html",
          "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<h1 style='font-size:2rem;text-align:center'>Arquivo não encontrado</h1>"
        );
        return;
      }

      server.setContentLength(file.size());
      server.sendHeader("Content-Type", "application/octet-stream");

      server.sendHeader(
        "Content-Disposition",
        "attachment; filename=\"Dados.csv\""
      );
      server.sendHeader("Connection", "close");
      server.send(200);

      uint8_t buffer[512];
      while (file.available()) 
      {
        size_t len = file.read(buffer, sizeof(buffer));
        server.client().write(buffer, len);
      }
      file.close();
    });

    server.begin();
    Serial.println("Servidor HTTP iniciado");

    createDir(LittleFS, "/mydir"); // Create a mydir folder
    writeFile(LittleFS, "/mydir/Dados.csv", "Data, Entradas, Saidas, Bateria, Erros"); // Create a hello1.txt file with the content "Hello1"
    listDir(LittleFS, "/", 1); // List the directories up to one level beginning at the root directory
    //deleteFile(LittleFS, "/mydir/hello1.txt"); //delete the previously created file
    //removeDir(LittleFS, "/mydir"); //delete the previously created folder
    //listDir(LittleFS, "/", 1); // list all directories to make sure they were deleted
    
    //writeFile(LittleFS, "/hello.txt", "Hello "); //Create and write a new file in the root directory
    //appendFile(LittleFS, "/hello.txt", "World!\r\n"); //Append some text to the previous file
    //readFile(LittleFS, "/hello.txt"); // Read the complete file
   // renameFile(LittleFS, "/hello.txt", "/foo.txt"); //Rename the previous file
   // readFile(LittleFS, "/foo.txt"); //Read the file with the new name
  //  deleteFile(LittleFS, "/foo.txt"); //Delete the file
    testFileIO(LittleFS, "/test.csv"); //Testin
    //deleteFile(LittleFS, "/test.txt"); //Delete the file
  
    Serial.println( "\n *******************   Test complete   *******************" ); 
}

void loop(){

  server.handleClient();

}