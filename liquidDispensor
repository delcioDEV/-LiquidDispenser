#include <WiFi.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <otadrive_esp.h>
#include <Arduino_MultiWiFi.h>

#define relay 2
#define button 12
#define buzzer 4
#define red_led 21
#define green_led 15


#define RFID_SDA 5
#define RFID_SCK 18
#define RFID_MOSI 23
#define RFID_MISO 19
#define RFID_RST 22

MFRC522 mfrc522(RFID_SDA, RFID_RST);

const char* ssid = "PowerSolution";                 // Replace with your WiFi network SSID
const char* password = "p@wers0lution2020";         // Replace with your WiFi network password
const char* server = "http://192.168.2.83/submit";  // Replace with your server's URL

MultiWiFi multi;

void initWiFi(void);
void post(String params);
void reconnectWiFi(void);
void blink(int led, int time, int sec);

void setup() {
  Serial.begin(115200);
  SPI.begin();         // Inicia  SPI bus
  mfrc522.PCD_Init();  // Inicia MFRC522

  // Configure the known networks (first one gets higher priority)
  multi.add("PowerSolution", "p@wers0lution2020");
  multi.add("Unitel T+ Net CT2065226245", "FFU47J0Y8V");

  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  initWiFi();

  OTADRIVE.setInfo("2a831bdb-fee1-4528-861a-915bd7868631", "v@1.1.1");
}

void ota() {
  if (OTADRIVE.timeTick(3600000)) {
    OTADRIVE.updateFirmware();
    Serial.print("update check running...");
    blink(red_led, 4, 600);  //blink the red led each sec when updating
  }
}

void loop() {

  ota();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (!mfrc522.PICC_IsNewCardPresent()) { return; }
  /* Select one of the cards */
  if (!mfrc522.PICC_ReadCardSerial()) { return; }
  /* Read data from the same block */
  //--------------------------------------------------
  //show UID on serial
  digitalWrite(buzzer, HIGH);
  delay(250);
  digitalWrite(buzzer, LOW);

  Serial.print("UID da tag :");
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    conteudo.concat(String(mfrc522.uid.uidByte[i], DEC));
  }

  Serial.println();
  Serial.print("Mensagem : ");


  post(conteudo);

  delay(100);
}

void post(String params) {
  // Send a POST request to the server with the tag UID
  // HTTPClient http;
  // http.begin(server);
  // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // String postData = "tag_uid=" + params+ "volume=0.35&fill=1";
  // Serial.print(server);
  // Serial.print(postData);
  // int httpResponseCode = http.POST(postData);
  // if (httpResponseCode > 0) {
  //   Serial.printf("POST request sent with tag UID: %s (HTTP response code: %d)\n", params.c_str(), httpResponseCode);
  // } else {
  //   Serial.printf("POST request failed with tag UID: %s (HTTP response code: %d)\n", params.c_str(), httpResponseCode);
  // }
  // http.end();

  HTTPClient http;
  http.begin(server);

  // Set the Content-Type header to multipart/form-data
  http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");

  // Create the form-data payload
  String payload = "";
  payload += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
  payload += "Content-Disposition: form-data; name=\"tag_uid\"\r\n\r\n";
  payload += params + "\r\n";
  payload += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
  payload += "Content-Disposition: form-data; name=\"volume\"\r\n\r\n";
  payload += "0.35\r\n";
  payload += "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
  payload += "Content-Disposition: form-data; name=\"fill\"\r\n\r\n";
  payload += "1\r\n";
  payload += "------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";

  // Send the POST request with the form-data payload
  int httpResponseCode = http.POST(payload);

  // Check if the request was successful
  if (httpResponseCode > 0) {
    Serial.printf("POST request sent with tag UID: %s (HTTP response code: %d)\n", params.c_str(), httpResponseCode);
  } else {
    Serial.printf("POST request failed with tag UID: %s (HTTP response code: %d)\n", params.c_str(), httpResponseCode);
  }

  // End the HTTP session
  http.end();
}

void initWiFi(void) {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.println("Aguarde");
  reconnectWiFi();
}

void reconnectWiFi(void) {
  if (WiFi.status() == WL_CONNECTED)
    return;

  //WiFi.begin(wifi_ssid, wifi_password);


  // Connect to the first available network
  Serial.println("Looking for a network...");
  if (multi.run() == WL_CONNECTED) {
    Serial.print("Successfully connected to network: ");
    Serial.println(WiFi.SSID());

  } else {
    Serial.println("Failed to connect to a WiFi network");
    blink(red_led, 3, 250);  // Blink the red LED 3 times with a delay of 250 milliseconds
  }
  blink(green_led, 3, 250);
  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void blink(int led, int time, int sec){
  for(int i=0; i<time; i++){
    digitalWrite(led, HIGH);
    delay(sec);
    digitalWrite(led, LOW);
    delay(sec);
  }
}
