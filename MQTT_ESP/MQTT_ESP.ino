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
#include <string.h>

//Comunications
const char* ssid = "lsel";
const char* password = "123456789";
const char* mqtt_server = "192.168.43.83";//ip de donde se encuentra alojado el servidor

//Pins
const int red = 0;
const int green = 1;
const int blue = 3;

//constants
const int max_duty = 1024;
const int min_duty = 1;
const int delay_disparo = 600; //en ms
unsigned long interval = 100;
unsigned long previousMillis = 0;

//Auxiliary variable
int hammer = 0;//se activa cuando se ha martillado al topo
int r = min_duty;
int g = max_duty;
int b = max_duty;

WiFiClient espClient;
PubSubClient client(espClient);

//Funcions//
/*Change among colours*/
void rainbow(int r, int g, int b);

/*Put a red colour when a mole has been beaten*/
void shot_fail(void);

void shot_success(void);

/*configuración WiFi*/
void setup_wifi();

/*interrupción*/
void callback(char* topic, byte* payload, unsigned int length);

/*reconexión*/
void reconnect();

void setup() 
{
  //Serial.begin(115200);
  setup_wifi();
  pinMode(0, OUTPUT);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() 
{
  unsigned long currentMillis = millis();

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  //cuenta el tiempo segun el intervalo establecido. Cuenta en ms
  if ((unsigned long)(currentMillis - previousMillis) >= interval)
  {
    previousMillis = millis();
            b -= 20;
            if (b <= 1)
            {
              b = 1000;
              g -= 20;
              if (g <= 1)
              {
                g = 1000;
                b = 1000;
                r += 20;
                if(r >= 1000)
                {
                  r = 1;
                }
              }
            }
          
          rainbow(r, g, b);
  }
}

void rainbow (int r, int g, int b)
{
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}

void shot_fail()
{
  analogWrite(green, min_duty);
  analogWrite(blue, min_duty);
  analogWrite(red, max_duty);//se enciende a máxima intensidad los leds rojos
  delay(delay_disparo);
}

void shot_success()
{
  analogWrite(green, max_duty);//se enciende a máxima intensidad los leds verdes
  analogWrite(blue, min_duty);
  analogWrite(red, min_duty);
  delay(delay_disparo);
}

void setup_wifi() 
{ 
  delay(10);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  //se activa si se ha recibido un martillazo
  if ((!strcmp(topic, "topo1/acierto") | !strcmp(topic, "topo2/acierto") | !strcmp(topic, "topo3/acierto")) & (char)payload[0] == '1') 
  {
    shot_success();
  } 
  else 
  {
    shot_fail();  
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      //Serial.println("connected");
      client.subscribe("MANDO/disparo");
      client.subscribe("topo1/acierto");
      client.subscribe("topo2/acierto");
      client.subscribe("topo3/acierto");
    } else {
      delay(5000);
    }
  }
}
