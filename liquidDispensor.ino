#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char* ssid = "**YOUR-WIFI-CREDENTIALS**";
const char* password = "**YOUR-WIFI-PASSWORD**";

WebServer server(80);

const int flowSensorPin = 39;
const int valvePin = 12;
const int LED = LED_BUILTIN;

const char* serverName = "http://192.168.2.87";
const int serverPort = 80;
const char* endpoint = "/AutoBeer/getdata.php";

volatile int pulseCount = 0;
float flowRate = 0.0;
float volume = 0.0;
float calibrationFactor = 5.8;



bool targetfill, fill = false;
float targetVolume = 0.0;
String targetTagUid = "";

float previousTargetVolume = 0.0;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void blink(int times, float interval);

void handleRoot() {
  String html = "<html><body><h1>AutoBeer Dispenser</h1>";
  html += "<form method='post' action='/submit'>";
  html += "<label for='tag_uid'>Enter the tag UID:</label><br>";
  html += "<input type='text' id='tag_uid' name='tag_uid'><br>";
  html += "<label for='volume'>Enter the desired volume (in l):</label><br>";
  html += "<input type='text' id='volume' name='volume'><br>";
  html += "<input type='submit' value='Submit'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSubmit() {
  bool fill = server.arg("fill");
  targetfill = fill;
  String tagUidStr = server.arg("tag_uid");
  targetTagUid = tagUidStr;
  String volumeStr = server.arg("volume");
  targetVolume = volumeStr.toFloat();
  Serial.print("New target volume: ");
  Serial.println(targetVolume);
  server.send(200, "text/plain", "Filling: " + String(targetfill) + " and volume set to " + volumeStr + " ml.");
}


void setup() {
  Serial.begin(115200);
  pinMode(flowSensorPin, INPUT_PULLUP);
  pinMode(valvePin, OUTPUT);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING);
  WiFi.config(IPAddress(192, 168, 2, 83), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0), IPAddress(8, 8, 8, 8));
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    blink(2,1);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println();
  Serial.print("Connected to WiFi with IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();
}

void loop() {
  blink(1,0.5);
  server.handleClient();

  // Check if target volume has changed
  if (targetVolume != previousTargetVolume) {
    // Update target volume
    previousTargetVolume = targetVolume;
    volume = 0.0;  // Reset volume counter
    Serial.print("New target volume: ");
    Serial.println(targetVolume);
  }


  // Dispense beer if target volume is set and not reached
  unsigned long startTime = millis();
  while (targetVolume > volume && targetfill) {
    digitalWrite(valvePin, HIGH);
   
    detachInterrupt(digitalPinToInterrupt(flowSensorPin));
        
        flowRate = ((float)pulseCount / calibrationFactor) * 60.0;
        volume += (flowRate / 60000.0)  ;
        pulseCount = 0;
        attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING);
        
      // Print flow rate and volume
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.print(" mL/s, Volume: ");
    Serial.print(volume);
    Serial.println(" mL");
    Serial.print(" mL, targerVolume: ");
    Serial.print(targetVolume);
    Serial.println(" mL");
    break;
  }

  // Set the fill variable to false if targetfill is false
  if (!targetfill) {
    Serial.println("fil stp");
   // digitalWrite(valvePin, LOW);
    fill = false;
    // Reset target fill flag and volume when target volume is reached
    targetfill = false;
    targetVolume = 0.0;
  }

  if (targetVolume <= volume) {
  Serial.println("fill stp");
  digitalWrite(valvePin, LOW);
  sendVolume(volume);
  fill = false;
  // Reset target fill flag and volume when target volume is reached
  targetfill = false;
  targetVolume = 0.0;

  // Reset all other variables to 0
  pulseCount = 0;
  flowRate = 0.0;
  volume = 0.0;
  
  targetTagUid = "";
  previousTargetVolume = 0.0;
}

}

void sendVolume(float volume) {
  HTTPClient http;
  String url = String(serverName) + String(endpoint);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  String payload = "{\"volume\": " + String(volume, 2) + "}";
  Serial.println("SERVER:");
  Serial.print(url);
  Serial.println("payload:");
  Serial.print(payload);
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void blink(int times,float interval){
  interval *=1000;
  for(int i=0;i<times;i++){
    digitalWrite(LED,HIGH);
    delay(250);
    digitalWrite(LED,LOW);
  }
}
