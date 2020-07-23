/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
const uint16_t port = 2020;
const char * host = "192.168.0.20"; // ip or dns

const char comando1[] = "leer";
const char* comando2 = "a";
const char* comando3 = "b";

static char* estado_swt = "LOW";
void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("casa LANnister", "P3r11JML20");

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void loop()
{
//    const uint16_t port = 80;
//    const char * host = "192.168.1.1"; // ip or dns
    static uint8_t band = 0;
    //Use la clase WiFiClient para crear conexiones TCP
    static WiFiClient client;
    if(band==0)
    {
      Serial.print("conectando a ");
      Serial.println(host);
     
      if (!client.connect(host, port)) {
        Serial.println("Connexion fallida.");
        Serial.println("Esperar 5 segundos antes de volver a intentar...");
        delay(5000);
        return;
      }
      else{
        Serial.print("se estableció la conexion a ");
        Serial.println(host);
        client.println("ESP32 conectado");
        band=1;
      }
    }

    if(!client.connected())
    {
      band=0;
      Serial.println("Desconectado, se perdió la conexión con el servidor");
      Serial.println("Esperar 5 segundos, para volver a establer la conexión...");
    }

    if((client.available()>0) && client.connected())
    {
      String line = client.readStringUntil('\r');
      //size_t mensg = client.available();
      //uint8_t sbuf[mensg];
      //client.readBytes(sbuf, mensg);
       if(line == comando1){
        Serial.print(host);
        Serial.println(" solicitó el estado de switch");
        client.print("Estado de switch: ");
        client.println(estado_swt);
       }
       else if(line == comando2){
        estado_swt = "LOW";
        Serial.print(host);
        Serial.println(" solicitó apagar el LED");
        client.println("LED off");
       }
       else if(line == comando3){
        estado_swt = "HIGH";
        Serial.print(host);
        Serial.println(" solicitó prender el LED");
        client.println("LED on");
       }
      //String line = client.readStringUntil('');
      //Serial.write(sbuf[0]);
    }
    /*
    Serial.print("Connecting to ");
    Serial.println(host);

    // Use la clase WiFiClient para crear conexiones TCP
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }*/

    //This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    //client.print("Enviar estos datos al servidor");
    //uncomment this line to send a basic document request to the server
    //client.print("GET /index.html HTTP/1.1\n\n");

  //int maxloops = 0;

  //espere a que la respuesta del servidor esté disponible
  /*
  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  
  if (client.available() > 0)
  {
    // lee una línea del servidor
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  else
  {
    Serial.println("client.available() timed out ");
  }
    Serial.println("Closing connection.");
    client.stop();
  
    Serial.println("Waiting 5 seconds before restarting...");
    delay(5000);*/
}
