#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

//how many clients should be able to telnet to this ESP32

#define MAX_SRV_CLIENTS 3
const char* ssid = "casa LANnister";
const char* password = "P3r11JML20";
uint8_t i,k;

WiFiServer server(2020);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  Serial.begin(115200);
  Serial.println("\nConnecting");

  wifiMulti.addAP(ssid, password);
  
  Serial.println("Connecting Wifi ");
  for (int loops = 10; loops > 0; loops--) {
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("WiFi connected ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    else {
      Serial.println(loops);
      delay(1000);
    }
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    delay(1000);
    ESP.restart();
  }

  //start UART and the server
  Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 2020' to connect");
}

void permitir_clientes_nuevos(){
  //check if there are any new clients
    if (server.hasClient()){
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          if (!serverClients[i]) Serial.println("available broken");
          serverClients[i].println("se ha añadido al grupo");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].remoteIP());
          
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        server.available().stop();
      }
    }
}

void comunicacion_clientes_grupo(){
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        if(serverClients[i].available()){
          size_t mensg = serverClients[i].available();
          uint8_t sbuf[mensg];
          serverClients[i].readBytes(sbuf, mensg);
          for(k = 0; k < MAX_SRV_CLIENTS; k++){
            if (serverClients[k] && serverClients[k].connected()){
              if(serverClients[k]!=serverClients[i]){
                serverClients[k].print(serverClients[i].remoteIP());
                serverClients[k].print(':');
                serverClients[k].write(sbuf, mensg);
                serverClients[k].println(' ');
              //Serial.write(sbuf, len);
              delay(1);
              }
            }
          }
        }
      }
      else {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
      }
    }
}

void mensaje_server_cliente(){
  if(Serial.available()){
      size_t len = Serial.available();
      uint8_t sbuf[len];
      Serial.readBytes(sbuf, len);
      //push UART data to all connected telnet clients
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        if (serverClients[i] && serverClients[i].connected()){
          serverClients[i].write(sbuf, len);
          serverClients[i].println(' ');
          //Serial.write(sbuf, len);
          //delay(1);
        }
      }
    }
}

void loop() {
  
  if (wifiMulti.run() == WL_CONNECTED) {
    //check if there are any new clients
    permitir_clientes_nuevos();
    //check clients for data--------recepcion de mensajes de los clientes
    comunicacion_clientes_grupo();
    mensaje_server_cliente();
    //check UART for data
  }
}
