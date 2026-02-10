# Firmware Sensor BeeCounter
Hardware - heltec-esp32-lora V2



##Bibliotecas:
ESPAsyncWebServer:
https://github.com/me-no-dev/ESPAsyncWebServer

AsyncTCP:
https://github.com/me-no-dev/AsyncTCP




##tamanho do arquivo

taxa de 5 min para salvar cada linha

60/5   = 12 linhas/hora
12*24  = 288 linhas/Dia
30*288 = 8640 linhas/mes
3 meses dá 25920 linhas


tamanho maximo do arquivo está em 767 kBytes


Em teste:
66,32kb -> 2514 linhas
então:

0,02638 kb por linha
25920 * 0,02638 = 683,77 kBytes

tem uma media de 6 meses de dados
 3 meses para Dados.csv
 3 meses para Backup_Dados.csv


##Tutoriais tomados como exemplo

https://blog.eletrogate.com/monitoramento-remoto-de-temperatura-utilizando-a-heltec-esp32-lora/

https://www.techcoil.com/blog/how-to-show-the-temperature-and-humidity-of-your-room-on-a-heltec-wifi-kit-32-attached-to-a-dht11-dht22-sensor/

Tutorial para utilizar firebase e esp32
https://www.youtube.com/watch?v=_ADdFH2K7SI
