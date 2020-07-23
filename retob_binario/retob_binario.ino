
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
int ledpin=5;
const uint16_t port = 2020;
const char * host = "192.168.0.20"; // ip or dns

const uint8_t comando1 = 1;//0000 0001 LEER
const uint8_t comando2 = 0;//0000 0000 APAGAR
const uint8_t comando3 = 255;//1111 1111 ENCENDER
static uint8_t num = 0;

static char* estado_swt = "abierto";
static WiFiClient client;
void setup()
{
    Serial.begin(115200);
    pinMode(ledpin, OUTPUT);
    digitalWrite(ledpin, HIGH);
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

void tarea_conexion_server(){
    static uint8_t band = 0;
  
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

}

void control_led(){
  if((client.available()>0) && client.connected())
    {
      num = client.read();
       if(num == comando1){
        Serial.print(host);
        Serial.println(" solicitó el estado de switch");
        client.print("Estado de switch: ");
        client.println(estado_swt);
        //client.println(digitalRead(ledpin));
       }
       else if(num == comando2){
        digitalWrite(ledpin,HIGH);
        estado_swt = "Abierto";
        Serial.print(host);
        Serial.println(" solicitó apagar el LED");
        client.println("LED off");
       }
       else if(num == comando3){
        digitalWrite(ledpin,LOW);
        estado_swt = "cerrado";
        Serial.print(host);
        Serial.println(" solicitó prender el LED");
        client.println("LED on");
       }
       else{
        client.println("comando no valido");
       }
    }
}

void loop()
{
    tarea_conexion_server();
    control_led();
}
