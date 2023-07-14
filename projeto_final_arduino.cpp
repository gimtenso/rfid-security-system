/*
RC522       MEGA
SDA         D9
SCK         D52
MOSI        D51
MISO        D50
IRQ         N/A
GND         GND
RST         D8
3.3V        3.3V
*/

// Bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Pinos
#define BUT_ON 3
#define LED_AZUL 4
#define LED_VERDE 5
#define LED_VERMELHO 6
#define BUZZER 7
#define RESET 8
#define SDA 9

// Variáveis
unsigned long lastTime = 0;
unsigned long lastTime2 = 0;
unsigned long debounceDelay = 35;
unsigned long delayCadastro = 4000;
int lastButton = LOW;
int button;
String tagCadastrada = ""; // Variável que armazenará o ID da Tag
String tagLida = "";
String tagLida2 = "";
bool buttonPressed = false;
bool verificado = false;
bool Cadastrada = false;

char envia;

Servo myservo;            // create servo object to control a servo
MFRC522 RFID(SDA, RESET); // Cria uma nova instância para o leitor e passa os pinos como parâmetro
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); // Inicializa a comunicação Serial
  SPI.begin();         // Inicializa comunicacao SPI
  RFID.PCD_Init();
  myservo.attach(10);            // attaches the servo on pin 9 to the servo
  pinMode(BUT_ON, INPUT);        // Inicializa o leitor RFID
  pinMode(LED_VERDE, OUTPUT);    // Declara o pino do led verde como saída
  pinMode(LED_VERMELHO, OUTPUT); // Declara o pino do led vermelho como saída
  pinMode(LED_AZUL, OUTPUT);     // Declara o pino do led azul como saída
  pinMode(BUZZER, OUTPUT);       // Declara o pino do buzzer como saída
  myservo.write(10);
}
void loop()
{
  debounce();
  lerTag();
  reset();
}
void lerTag()
{
  // Verifica se existe uma Tag presente
  tagLida = "";
  if (RFID.PICC_IsNewCardPresent() && RFID.PICC_ReadCardSerial())
  {
    digitalWrite(LED_AZUL, HIGH);
    for (int i = 0; i < RFID.uid.size; i++)
    {
      tagLida.concat(String(RFID.uid.uidByte[i], DEC));
      delay(100);
    }
    RFID.PICC_HaltA(); // Interrompe a comunicação com a tag
    if (tagCadastrada == tagLida)
    {
      verificaTag();
    }
    else
    {
      // Serial.println();
      Serial.print("Tag lida: ");
      Serial.println(tagLida);
      Serial1.write('0'); // esse número que vamos escrever vai servir para fazermos um case dentro do pic para sabermos qual ação tomar.//0 = printar tag lida no display
      cadastraTag();
    }
  }
  else
  {
    digitalWrite(LED_AZUL, LOW);
  }
}

void cadastraTag()
{
  debounce();
  if (buttonPressed == true)
  {
    tagCadastrada = tagLida;
    Cadastrada = true;
    Serial.print(tagCadastrada);
    Serial.println(" cadastrada com sucesso!");
    Serial1.write('1'); // esse número que vamos escrever vai servir para fazermos um case dentro do pic para sabermos qual ação tomar.//1 = printar tag cadastrad
    buttonPressed = false;
    verificado = true;
  }
  else
  {
    Serial.print(tagLida);
    Serial.println(" não cadastrada");
    Serial1.write('2'); // esse número que vamos escrever vai servir para fazermos um case dentro do pic para sabermos qual ação tomar.//2 = printar tag não cadastrada
    Cadastrada = false;
  }
  verificaTag();
}

void verificaTag()
{
  if (tagCadastrada == tagLida)
  {
    verificado = true;
    Serial.println("Tag cadastrada.");
    Serial1.write('3'); // esse número que vamos escrever vai servir para fazermos um case dentro do pic para sabermos qual ação tomar.//3 = printar tag cadastrada
    efeitoPermitido();
    tagLida = "";
    lerTag();
  }
  else
  {
    Serial.println("Tag não cadastrada.");
    efeitoNegado();
    verificado = false;
  }
  lerTag();
}

void efeitoPermitido()
{
  int qtd_bips = 2; // definindo a quantidade de bips
  for (int j = 0; j < qtd_bips; j++)
  {
    // Ligando o buzzer com uma frequência de 1500 hz e ligando o led verde.
    tone(BUZZER, 1500);
    digitalWrite(LED_VERDE, HIGH);
    delay(100);
    // Desligando o buzzer e led verde.
    noTone(BUZZER);
    digitalWrite(LED_VERDE, LOW);
    delay(100);
  }
  myservo.write(190);
  delay(3000);
  myservo.write(10);
  lerTag();
}

void efeitoNegado()
{
  int qtd_bips = 1; // definindo a quantidade de bips
  for (int j = 0; j < qtd_bips; j++)
  {
    // Ligando o buzzer com uma frequência de 500 hz e ligando o led vermelho.
    tone(BUZZER, 500);
    digitalWrite(LED_VERMELHO, HIGH);
    delay(500);
    // Desligando o buzzer e o led vermelho.
    noTone(BUZZER);
    digitalWrite(LED_VERMELHO, LOW);
    delay(500);
    myservo.write(10);
  }
}

void debounce()
{
  int reading = digitalRead(BUT_ON);
  if (reading != lastButton)
  {
    lastTime = millis();
  }
  if ((millis() - lastTime) > debounceDelay)
  {
    if (reading != button)
    {
      button = reading;
      if (button == HIGH)
      {
        buttonPressed = true;
      }
    }
  }
  lastButton = reading;
}

void reset()
{
  char c = Serial.read();
  if (c == 'r')
  {
    tagCadastrada = "";
    Serial.println(tagCadastrada);
    lerTag();
  }
  else
  {
  }
}
