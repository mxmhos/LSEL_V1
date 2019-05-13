/*
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#define R 0
#define G 1
#define B 2

#define ON 1
#define OFF 0

const char* ssid = "Especial_MCHP";
const char* password = "M15SEB304";
const char* mqtt_server = "172.16.2.4";//ip de donde se encuentra alojado el servidor

/*const char* ssid = "lsel";
const char* password = "123456789";
const char* mqtt_server = "192.168.43.148";//ip de donde se encuentra alojado el servidor*/

const int port = 1883;

long lastMsg = 0;
char msg[50];
int value = 0;
int estado = OFF;
int shoot_rcv = 0;
Servo servo;

WiFiClient espClient;
PubSubClient client(espClient);



//Una vez implementado correctamente en el ESP hay que borrar la comunicacion serial
void setup() {
  servo.attach(R);//PWM para el control del servo que mueve a los topos conectado al PIN 0
  servo.writeMicroseconds(0);

  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {

  /*if (!client.connected()) {
    reconnect();
  }*/

    servo.writeMicroseconds(0);//servo.write(0);
    
  }

}








