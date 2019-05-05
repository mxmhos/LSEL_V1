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

//const char* ssid = "Especial_MCHP";
//const char* password = "M15SEB304";
//const char* mqtt_server = "192.168.1.202";//ip de donde se encuentra alojado el servidor

const char* ssid = "lsel";
const char* password = "123456789";
const char* mqtt_server = "192.168.43.148";//ip de donde se encuentra alojado el servidor

const int port = 1883;

long lastMsg = 0;
char msg[50];
int value = 0;
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
  attachInterrupt(PHOTO_RCV,shoot,FALLING); // Interrupcion con el flanco de bajada  
  Serial.begin(115200);  
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();//Mantiene la sesion con el servidor MQTT
  if (shoot_rcv) {
    /*snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);*/
    client.publish("sonido", "1");
    shoot_rcv = 0;
  }
}
void setup_wifi() {

  delay(100);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(1, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(1, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("topos1", "5");
      // ... and resubscribe
      client.subscribe("topos/");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void shoot(){
  shoot_rcv = 1;
}

