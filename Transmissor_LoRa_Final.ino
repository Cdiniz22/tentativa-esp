/* Includes para header files das biliotecas do OLED */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include <CRC32.h>

// configuração do GPS
#define GPS_RX 38
#define GPS_TX 17
#define GPS_Serial_Baud 9600
#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;

SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

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

/* Variaveis */
int contador = 0;
float flat, flon;

#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18

#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */





/* Funcao setup */
void setup() 
{
    /* inicializa display OLED */
    Serial.begin(9600);
    gpsSerial.begin(GPS_Serial_Baud);

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

    if (!LoRa.begin(BAND))
    {
      Serial.println("[LoRa Sender] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");
      Serial.println(LoRa.begin(BAND));
      display.setCursor(0, OLED_LINE1);
      display.println("Comunicacao Falhou");
      display.display();
      while(1);
    }

    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa ok");
    display.setCursor(0, OLED_LINE1);
    Serial.println(LoRa.begin(BAND));
    display.println("Comunicacao Realizada!!");
    display.println(String(LoRa.begin(BAND)));
    display.display();
    delay(2000);

    CRC32 crc;
    crc.update(data.c_str(), data.length()); // Atualiza o CRC com os dados
    int crcValue = crc.finalize();
}      

/* Programa principal */
void loop() 
{
  bool newData = false;
  unsigned long chars;
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      Serial.write(c); //apague o comentario para mostrar os dados crus
      //Serial.print(gps.encode(c));
      if (gps.encode(c)) // Atribui true para newData caso novos dados sejam recebidos
        newData = true;
    }
  }
  if (newData)
  {
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat,6);
    Serial.print(" LON=");
    Serial.print(flon,6);
  }


    display.clearDisplay();
     
    display.setCursor(0, OLED_LINE1);
    display.println("ENVIANDO PACOTE...");
    
    display.setCursor(0, OLED_LINE2);
    display.print("Tempo ligado: ");

    display.setCursor(0, OLED_LINE3);
    display.print(contador);
    display.println("s");

   
    display.setCursor(0, OLED_LINE4);
    display.print(flat,6);

    display.setCursor(0, OLED_LINE5);
    display.print(flon,6);

           
    display.display();

    
    LoRa.beginPacket();

    LoRa.print(flat,6);
    LoRa.print(" ");
    LoRa.print(flon,6);
    LoRa.print("@");
    LoRa.print(contador);
    LoRa.print("|");
    LoRa.print(crcValue);


    LoRa.endPacket();



    contador++;
}