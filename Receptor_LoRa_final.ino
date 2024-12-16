/* Includes para header files das biliotecas do OLED */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFiManager.h>
#include <String>
#include <iostream>
#include <vector>
#include <Arduino_CRC32.h>
using namespace std;


/* Endereço I2C do display */
#define OLED_ADDR   0x3c

/* distancia, em pixels, de cada linha em relacao ao topo do display */
#define OLED_LINE1     0
#define OLED_LINE2     10
#define OLED_LINE3     20
#define OLED_LINE4     30
#define OLED_LINE5     40

/* Configuração da resolucao do display (este modulo possui display 128x64) */
#define SCREEN_WIDTH    128 
#define SCREEN_HEIGHT   64  

/* Objeto do display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16);

/* */
std::vector<std::pair<float, float>> monitora = {};
/* */
String htmlpage;
WiFiServer server(80);

/* Variaveis */
int contador = 0;
String desejo;
String mensagem;
float flate,flone;
String flat, flon, conta, crcrece;
int perdido, correto, totalpac;

Arduino_CRC32 crc;



#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18

#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */

#define DIM_PACOTE   240

void setup() {
    Serial.begin(9600);

    /*Configuracao WIFI*/
    WiFiManager wm;
    bool res; 
    res = wm.autoConnect("AutoConnectAP", "12345678");
    if (!res) {
      Serial.println("Failed to connect");
      ESP.restart();
    }
    else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    }
    

    /*configuracao do LoRa*/
    Wire.begin(4, 15);
    SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
    LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);

     if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
        Serial.println("Display OLED: falha ao inicializar");   
    else
    {
        Serial.println("Display OLED: inicializacao ok");            
        
        /* Limpa display e configura tamanho de fonte */
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
    } 

   if(!LoRa.begin(BAND)){
     display.setCursor(0, OLED_LINE1);
     display.println("Falhou");
     display.display(); 
     while(1);
    }   
    display.setCursor(0, OLED_LINE1);
    display.println("Deu certo");
    display.setCursor(0, OLED_LINE2);
    display.println("Esperando Dados...");
    display.display();
    delay(1000);

  LoRa.receive();
  
  server.begin();

 
}

void loop() {
  int packetSize = LoRa.parsePacket();

    if (packetSize){
        Serial.println("[LoRa Receiver] Há dados a serem lidos"); 
        mensagem = "";

        while (LoRa.available()){
          mensagem += (char)LoRa.read();
        }
        


        Serial.println(mensagem);
        int sel1 = mensagem.indexOf(' ');
        int sel2 = mensagem.indexOf('@');
        int sel3 = mensagem.indexOf('|');

        flat = mensagem.substring(0,sel1);
        flon = mensagem.substring(sel1+1, sel2);
        conta = mensagem.substring(sel2+1, sel3);
        crcrece = mensagem.substring(sel3+1);

        
        crc.reset();  // Reseta o estado do CRC
        crc.update(crcrece.c_str(), crcrece.length());
        uint32_t crccalc = crc.finalize();  // Obtém o valor final do CRC32


        if (crccalc == crcrece.toInt()){
          correto += 1;
        } else {
          perdido += 1;
        }
        totalpac = correto + perdido;

        display.clearDisplay();   
        display.setCursor(0, OLED_LINE1);
        display.print("Dado recebido: ");
        display.setCursor(0, OLED_LINE2);
        display.print("LAT= " + flat);
        display.setCursor(0, OLED_LINE3);
        display.print("LON= " + flon);
        display.setCursor(0, OLED_LINE4);
        display.print(conta + "   " + totalpac + "  perd: " + perdido);
        display.setCursor(0, OLED_LINE5);
        display.print(WiFi.localIP());

        display.display();


        WiFiClient client = server.available();  // listen for incoming clients
        Serial.print(WiFi.localIP());

        if (client) { 
          Serial.print("opaa");                           // if you get a client,
          Serial.println("New Client.");           // print a message out the serial port
          String currentLine = "";                // make a String to hold incoming data from the client
          while (client.connected()) {           // loop while the client's connected
              if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                  // if the current line is blank, you got two newline characters in a row.
                  // that's the end of the client HTTP request, so send a response:
                  if (currentLine.length() == 0) {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                    client.println();

                    // the content of the HTTP response follows the header:
                    //client.print("<h1>    HELLO SAO GONCALO    </h1>");
                    //client.print("Clique <a href=\"/L\">AQUI</a> para o Led ligar.<br>");
                    //client.print("Click <a href=\"/H\">AQUI</a> para o Led desligar.<br>");
                    //client.print("Click <a href=\"/R\">AQUI</a> para resetar a conexao WiFi.<br>");
                  htmlpage = "";
                  htmlpage = R"rawliteral(<!DOCTYPE html>
                  <html>
                  <head>
                    <title>Rastreador GPS</title>
                    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.3/dist/leaflet.css" />
                    <script src="https://unpkg.com/leaflet@1.9.3/dist/leaflet.js"></script>
                    <style>
                      #map {
                        height: 450px; 
                      }
                    </style>
                  </head>
                  <body>
                    <h1 style="text-align: center;">Rastreador GPS</h1>
                    <div id="map"></div>
                    <script>
                      const positions =[)rawliteral";

            for (int i = 0; i < monitora.size(); i++) {
                    htmlpage += "[" + String(monitora[i].first, 6) + "," + String(monitora[i].second, 6) + "]";
                    if (i < monitora.size() - 1) htmlpage += ",";
                }

                    htmlpage += "]";

                    htmlpage += R"rawliteral(
                      
                    const map = L.map('map').setView([)rawliteral"; 
                    htmlpage += flat;
                    htmlpage += ",";
                    htmlpage += flon;
                    htmlpage += R"rawliteral(], 13);
                    L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
                      maxZoom: 19,
                    }).addTo(map); 
                    const marker = L.marker([)rawliteral"; 
                    htmlpage+= flat;
                    htmlpage+= ",";
                    htmlpage+= flon;
                    htmlpage+=R"rawliteral( ]).addTo(map);
                    const polyline = L.polyline(positions,{color:'#F00'})
                    polyline.addTo(map)
                  </script> 
                </body> 
                </html>)rawliteral"; 
                    
        client.print(htmlpage);
        client.print("<a href=\"/\">LOCALIZACAO</a><br>");
        client.print("<a href=\"/R\">REINICIAR</a><br>");

        // The HTTP response ends with another blank line:
        client.println();
        // break out of the while loop:
          break;
        } else {    // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }

      // Check to see if the client request was "GET /H" or "GET /L":
      if (currentLine.endsWith("GET /")) { 
      }
  
      if (currentLine.endsWith("GET /R")) {
        WiFiManager wm;
        wm.resetSettings();
        ESP.restart();   
      }
    }
    }
          // close the connection:
          client.stop();
          Serial.println("Client Disconnected.");
        }
          }
}
