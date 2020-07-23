 #include <WiFi.h>
 #include <WiFiUdp.h>

 const char* ssid = "casa LANnister";
 const char* password = "P3r11JML20";
 WiFiUDP udpDevice;
 uint16_t localUdpPort = 3001;
 uint32_t previousMillis = 0;

const uint8_t comando1 = 1;//leer estado del switch
const uint8_t comando2 = 0;//apagar
const uint8_t comando3 = 255;//encender

 #define ALIVE 1000
 #define D0 5
 #define D8 18

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
         Serial.print(".");
     }
     Serial.println("");
     Serial.println("WiFi connected");
     // Print the IP address
     Serial.println(WiFi.localIP());
     udpDevice.begin(localUdpPort);
 }


 void networkTask() {
     //uint8_t data;
     uint8_t packetSize = udpDevice.parsePacket();
     char* estado = "apagado";
     char* estado_switch= "abierto";
     //String line = udpDevice.readStringUntil('\r');
     uint8_t men = udpDevice.read();
     if (packetSize>0){
      
        // String line = udpDevice.readStringUntil('\r');
         //ta = udpDevice.read();
         if (men == comando3) {
          estado = "encendido";
          estado_switch="cerrado";
          digitalWrite(D0, LOW);
          udpDevice.print("led ");
          udpDevice.println(estado);
          udpDevice.endPacket();
         } 
         else if (men == comando2) {
          estado = "apagado";
          digitalWrite(D0, HIGH);
          estado_switch="abierto";
          udpDevice.print("led ");
          udpDevice.println(estado);
          udpDevice.endPacket();
         }
         else if (men == comando1) {
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.print("Estado del switch ");
          udpDevice.println(estado_switch);
          udpDevice.endPacket();
         }
         else{
          udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
          udpDevice.println("comando no valido ");
          udpDevice.endPacket();
         }
     }
 }

 void loop() {
     networkTask();
 }
