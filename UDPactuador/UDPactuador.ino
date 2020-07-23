 #include <WiFi.h>
 #include <WiFiUdp.h>

 const char* ssid = "casa LANnister";
 const char* password = "P3r11JML20";
 WiFiUDP udpDevice;
 uint16_t localUdpPort = 3001;
 uint32_t previousMillis = 0;
 #define ALIVE 1000
 #define D0 5
 #define D8 18

 void setup() {
     pinMode(D0, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
     digitalWrite(D0, HIGH);
     //pinMode(D8, OUTPUT);
     //digitalWrite(D8, LOW);
     Serial.begin(115200);
     Serial.println();
     Serial.println();
     Serial.print("Connecting to ");
     Serial.println(ssid);

     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);

     while (WiFi.status() != WL_CONNECTED) {
         delay(2000);
         Serial.print(".");
     }
     Serial.println("");
     Serial.println("WiFi connected");
     // Print the IP address
     Serial.println(WiFi.localIP());
     udpDevice.begin(localUdpPort);
 }


 void networkTask() {
     uint8_t data;
     uint8_t packetSize = udpDevice.parsePacket();
     String estado = "apagado";
     if (packetSize) {
         data = udpDevice.read();
         if (data == '1') {
          estado = "encendido";
          digitalWrite(D0, LOW);
         } 
         else if (data == '0') {
          estado = "apagado";
          digitalWrite(D0, HIGH);
         }
         // send back a reply, to the IP address and port we got the packet from
         udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
         udpDevice.write(data);
         udpDevice.print(' ');
         udpDevice.print("led ");
         udpDevice.println(estado);
         udpDevice.endPacket();
     }
 }

 void aliveTask() {
     uint32_t currentMillis;
     static uint8_t ledState = 0;
     currentMillis  = millis();
     if ((currentMillis - previousMillis) >= ALIVE) {
         previousMillis = currentMillis;
         if (ledState == 0) digitalWrite(D8, HIGH);
         else digitalWrite(D8, LOW);
     }
 }

 void loop() {
     networkTask();
     aliveTask();
 }
