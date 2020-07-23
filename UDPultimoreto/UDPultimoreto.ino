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
     pinMode(D8, INPUT);
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
     static String estado = "apagado";
     //static String estado_switch = "abierto";
     if (packetSize) {
         data = udpDevice.read();
         if (data == 'a') {
          estado = "encendido";
          //estado_switch = "cerrado";
          digitalWrite(D0, LOW);
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.print("led ");
          udpDevice.println(estado);
          udpDevice.endPacket();
         } 
         else if (data == 'b') {
          estado = "apagado";
          //estado_switch = "abierto";
          digitalWrite(D0, HIGH);
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.print("led ");
          udpDevice.println(estado);
          udpDevice.endPacket();
         }
         else if (data =='l'){
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.print("Estado del switch ");
          udpDevice.println(digitalRead(D8));
          udpDevice.endPacket();
         }
         else{
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.println("comando no valido ");
          udpDevice.endPacket();
         }
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
