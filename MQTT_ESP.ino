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

#define PHOTO_RCV 2
#define MV_TOPO 0
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

void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnect();
void shoot();

//Una vez implementado correctamente en el ESP hay que borrar la comunicacion serial
void setup() {
  pinMode(PHOTO_RCV,INPUT);//Entrada del fototransistor PIN 2
  servo.attach(MV_TOPO,1000,2000);//PWM para el control del servo que mueve a los topos conectado al PIN 0
  servo.writeMicroseconds(1000);
  attachInterrupt(PHOTO_RCV,shoot,FALLING); // Interrupcion con el flanco de bajada   
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();//Mantiene la sesion con el servidor MQTT
  if ((shoot_rcv == 1) && (estado == ON)) {
    /*snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);*/
    estado = OFF;
    client.publish("TOPO/sonido", "1");
    servo.writeMicroseconds(1000);//servo.write(0);
    shoot_rcv = 0;
  }

}
void setup_wifi() {

  delay(100);
  // We start by connecting to a WiFi network

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  if ((char)payload[0] == '1') {
    estado = ON;
    servo.writeMicroseconds(2000);
    attachInterrupt(PHOTO_RCV,shoot,FALLING);
    //servo.write(180);   // Topo Sale
  } else if((char)payload[0] == '0') {
    estado = OFF;
    servo.writeMicroseconds(1000);//servo.write(0);  // Topo entra
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      // ... and resubscribe
      client.subscribe("TOPO/topo1");
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void shoot(){
  detachInterrupt(digitalPinToInterrupt(PHOTO_RCV));
  shoot_rcv = 1;
}

