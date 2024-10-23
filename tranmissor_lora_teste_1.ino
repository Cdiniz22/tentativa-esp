#include "heltec.h"
#include "WiFi.h"

/*Parte do LORA*/

#define DisplayEstado true
#define LoraEstado   true
#define SerialEstado true 
#define AmpEtado true
#define BANDA  433E6
/*
433E6
868E6
915E6
*/

/*Parte do WIFI*/
#define SSid "Seu_SSID"
#define Senha "Sua senha"

int counter = 0;



WiFiServer servidor(80);


void setup() {

  Heltec.begin(DisplayEstado, LoraEstado, SerialEstado,AmpEstado, BANDA);
  Heltec.display -> clear();

  SetupWifi();
  SetupLora();

  Heltec.display -> drawString(0,40, "Setup concluido!");

  servidor.begin(); 

  Heltec.display -> drawString(0,50, "Servidor:" + Wifi.localIP().toString());
  Heltec.display -> clear():
  delay(1000);


}

void loop() {
  /*TrataRequisicao();
  enviarPacote(); */
  LoRa.beeingPacket();
  LoRa.print("heloo World");
  LoRa.print(counter);
  LoRa.endPacket();
  /*LoRa.write()*/

  counter ++;
  delay(500);

}


void SetupWiFi(){
  WiFi.disconnect(true);
  delay(100);
  IPAddress ip(192, 168, 0, 119);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.config(ip,gateway,subnet);
  WiFi.begin(SSid,Senha)
  delay(100)
  byte count = 0;

  while (WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
    Heltec.display -> drawString(0,0, "conectando wifi.....");
    Heltec.display -> display();

  }

  if(WiFi.status() == WL_CONNECTED){
    Heltec.display -> drawString(0,10, "Conexao wifi ok!!!");
    Heltec.display -> display());
    delay(100);
  }
  else{
    Heltec.display -> drawString(0,10, "Falha na Conexao wifi!");
    Heltec.display -> display();
    delay(100);
  }

}

void SetupLora(){
  Heltec.display -> drawString(0,20, "Configurando Lora...");
  Heltec.display -> display());
  delay(100);

  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(6);
  LoRa.setSyncWord(0x12);
  LoRa.crc();
  Heltec.display -> drawString(0,30, "Lora Configurado!!");
  Heltec.display -> display());  
  delay(100);

}

void TrataRequisicao(){
  WiFiClient cliente = servidor.available();

  if(!cliente) return;

  Heltec.display -> clear();
  Heltec.display -> drawString(0,0, "Servidor: " +WiFi.localIP().toString());
  Heltec.display -> drawString(0,10, "Cliente Conectado");
  Heltec.display -> display());  

  while(cliente.connected()){
    if (cliente.available()){
      String req = cliente.ReadStringUntil('\r');

      if(req.indexOf("acao=0") != -1){
        Atuar = true;
        EstadoBomba = !EstadoBomba;
      }
      else if(req.indexOf("acao=1") != -1){

        Atuar = true;
        EstadoAquecedor = !EstadoAquecedor
      }

      String html = sfjsh;
      cliente.print(html);
      cliente.flush()
    }
    delay(20);
    cliente.stop();
  }
  Heltec.display -> clear();
  Heltec.display -> drawString(0,0, "Servidor: " +WiFi.localIP().toString());
  Heltec.display -> drawString(0,10, "Cliente Conectado");
  Heltec.display -> display());  
}

void enviarPacote(){


}