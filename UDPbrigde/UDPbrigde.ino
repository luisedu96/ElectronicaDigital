#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "casa LANnister";
const char* password = "P3r11JML20";
WiFiUDP udpDevice;
uint16_t localUdpPort = 3000;
//uint16_t UDPPort = ?;

#define MAX_LEDSERVERS 3
const char* hosts[MAX_LEDSERVERS] = {"192.168.20", "192.168.0.20", "192.168.0.20"};
const uint16_t UDPPort[MAX_LEDSERVERS] = {3001, 3002, 3003};

#define SERIALMESSAGESIZE 3
uint32_t previousMillis = 0;
#define ALIVE 1000
#define D0 5

void setup() {
  pinMode(D0, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(D0, HIGH);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  udpDevice.begin(localUdpPort);
}

void networkTask() {
  uint8_t LEDServer = 0;
  uint8_t LEDValue = 0;
  uint8_t syncChar;

  // Serial event:
  if (Serial.available() >= SERIALMESSAGESIZE) {
    LEDServer = Serial.read() - '0';
    LEDValue = Serial.read();
    syncChar = Serial.read();
    if (((LEDServer == 0) || (LEDServer > 3))||((LEDValue -'0') > 1)){
      Serial.println("Servidor inválido (seleccione 1,2,3)");
      return;
    }
    if (syncChar == '*') {
      udpDevice.beginPacket(hosts[LEDServer - 1] , UDPPort[LEDServer - 1]);
      udpDevice.write(LEDValue);
      udpDevice.endPacket();
    }
  }
  // UDP event:
  uint8_t packetSize = udpDevice.parsePacket();//tamaño de respuesta
  if (packetSize) {
    Serial.print("Data from: ");
    Serial.print(udpDevice.remoteIP());
    Serial.print(":");
    Serial.print(udpDevice.remotePort());
    Serial.print(' ');
    for (uint8_t i = 0; i < packetSize; i++) {
      Serial.write(udpDevice.read());
    }
    Serial.println(' ');
  }
}

void aliveTask() {
  uint32_t currentMillis;
  static uint8_t ledState = 0;
  currentMillis  = millis();
  if ((currentMillis - previousMillis) >= ALIVE) {
    previousMillis = currentMillis;
    if (ledState == 0) {
      digitalWrite(D0, HIGH);
      ledState = 1;
    }
    else {
      digitalWrite(D0, LOW);
      ledState = 0;
    }
  }
}

void loop() {
  networkTask();
  aliveTask();
}
