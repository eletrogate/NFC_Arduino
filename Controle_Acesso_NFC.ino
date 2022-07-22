/*
  Programa de Controle de Acesso para etiqueta NFC NTAG213
  Blog Eletrogate - http://blog.eletrogate.com/
  Arduino UNO - IDE 1.8.5 - Modulo RFID RC522
  Display LCD 16x2 I2C
  Gustavo Murta   09/maio/2018
  Biblioteca MFRC522 https://github.com/miguelbalboa/rfid
  Adaptado de http://blog.eletrogate.com/kit-rfid-com-arduino-sistema-de-controle-de-acesso/
*/

#include <SPI.h>                              // biblioteca SPI 
#include <MFRC522.h>                          // biblioteca do modulo RC522
#include <LiquidCrystal_I2C.h>                // biblioteca do display LCD com I2C 

#define SDA_PIN 10                            // pino SDA RC522 = D10 Arduino
#define RST_PIN 9                             // pino RST RC522 = D9  Arduino
#define TAGLiberada " 04 58 F1 72 20 4E 80"   // UID da etiqueta NFC liberada 
#define AcionaBuzzer 7                        // buzzer = D7 Arduino
#define BeepsLiberado 1                       // 1 bip = acesso liberado
#define BeepsNegado 2                         // 2 bips = acesso negado 

MFRC522 mfrc522(SDA_PIN, RST_PIN);    // cria uma instância MFRC522
LiquidCrystal_I2C lcd(0x3F, 16, 2);   // configuração do display LCD 16x2 I2C - mude para  0x27 se não funcionar

char st[20];                          // string com 20 caracteres
String UID = "";                      // string da identificação UID

void setup()
{
  pinMode(AcionaBuzzer, OUTPUT);
  Serial.begin(9600);                // monitor serial a 9600 Bps
  SPI.begin();                       // inicializa comunicação SPI
  mfrc522.PCD_Init();                // inicializa modulo RC522

  lcd.init();                        // inicializa display LCD
  lcd.backlight();                   // acende o led backlight do LCD
  StandardMessage();                 // imprime mensagem no LCD
}

void StandardMessage()
{
  lcd.clear();                       // limpa tela do LCD
  lcd.setCursor(3, 0);               // cursor na coluna 3 linha 0
  lcd.print("Aproxime a");           // print no LCD
  lcd.setCursor(2, 1);               // cursor na coluna 2 linha 1
  lcd.print("Etiqueta NFC");         // print no LCD
}

void getUID()
{
  Serial.print("UID da tag NFC : ");                            // imprime no monitor serial
  for (byte i = 0; i < mfrc522.uid.size; i++)                   // leitura da identificação UID da NFC
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");   // imprime os bytes
    Serial.print(mfrc522.uid.uidByte[i], HEX);                  // imprime UID em hexadecimal
    if ( mfrc522.uid.uidByte[i] < 0x10)                         // se byte menor do que 16
      UID.concat(" 0");                                         // insere um zero
    else                                                        // senão
      UID.concat(" ");                                          // insere um espaço

    UID.concat(String(mfrc522.uid.uidByte[i], HEX));            // concatena todos os bytes da UID
  }
  UID.toUpperCase();                                            // converte em maiusculos
  Serial.println(" ");                                          // imprime espaço
}

void BuzzerBeeps(int NumeroBeeps)
{
  int i;
  for (i = 0; i < NumeroBeeps; i++)
  {
    digitalWrite(AcionaBuzzer, HIGH);
    delay(150);                            // atraso de 0,15 segundos
    digitalWrite(AcionaBuzzer, LOW);
    delay(150);                            // atraso de 0,15 segundos
  }
}

void loop()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())   // verifica a presença de etiqueta
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())     // leitura da etiqueta NFC
  {
    return;
  }
  UID = "";                            // limpa o registro de identificação UID
  getUID();                            // lê e formata a identificação UID
  if (UID == TAGLiberada)              // verifica a liberação da etiqueta
  {
    lcd.clear();                       // limpa tela do LCD
    lcd.setCursor(0, 0);               // cursor na coluna 0 linha 0
    lcd.print("Acesso Liberado!");     // print no LCD
    BuzzerBeeps(BeepsLiberado);
    delay(500);                        // atraso de 0,5 segundos
    StandardMessage();                 // print da mensagem no LCD
  }
  else
  {
    lcd.clear();                       // limpa tela do LCD
    lcd.setCursor(1, 0);               // cursor na coluna 1 linha 0
    lcd.print("Acesso Negado!");       // print no LCD
    BuzzerBeeps(BeepsNegado);
    delay(500);                        // atraso de 0,5 segundos
    StandardMessage();                 // print da mensagem no LCD
  }
  delay(500);                          // atraso de 0,5 segundos
}
