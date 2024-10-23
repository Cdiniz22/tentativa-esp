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
void setup(){

  Heltec.begin(DisplayEstado, LoraEstado, SerialEstado,AmpEstado, BANDA);
  SetupLora();
  delay(100);

  Heltec.display -> clear());
  Heltec.display -> drawString(0,0, "Setup conluido!");
  Heltec.display -> drawString(0,10, "aguardando dados...");
  Heltec.display -> display());


}

void loop(){
  int packetSize = LoRa.parsePacket():
  if (packetSize){
    while (LoRa.available()){
      String LoRaData = LoRa.readString();
    }
    Heltec.display -> clear();
    Heltec.display -> drawString(0,0, LoRaData);
    Heltec.display -> drawString(0,10, "RSSI: "+ String(LoRa.packetRssi()));
    Heltec.display -> display());

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