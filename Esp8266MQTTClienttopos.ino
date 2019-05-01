#include "EspMQTTClient.h"

void onConnectionEstablished();

/*
EspMQTTClient client(
  "ssid",                 // Wifi ssid
  "pass",                 // Wifi password
  onConnectionEstablished,// Connection established callback
  "ip",                   // MQTT broker ip
  1883,                   // MQTT broker port
  "mqttusr",              // MQTT username
  "mqttpass",             // MQTT password
  "test",                 // Client name
  true,                   // Enable web updater
  true                    // Enable debug messages
);
*/

EspMQTTClient client(
//  "MiFibra-B160",                 // Wifi ssid
 // "bFybSTXn",                 // Wifi password
   
  onConnectionEstablished,// MQTT connection established callback
  //"192.168.1.134"                    // MQTT broker ip
"172.20.10.4"                    // MQTT broker ip
);

void setup()
{
  Serial.begin(115200);
 //attachInterrupt(2,shoot,FALLING);
}

void onConnectionEstablished()
{
  // Se usa para subscribirse a "proyecto/topos/topoquedesees" y muestra un mensaje en el serial
  client.subscribe("proyecto/topos/topo1", [](const String & payload) {
    Serial.println(payload);
  });

  // Publica un mensaje en "proyecto/topos/topoquedesees"
  client.publish("proyecto/topos/topo1", "Estado:1");

  // ejecuta instrucciones con tiempo de espera
  client.executeDelayed(5 * 1000, []() {
    client.publish("proyecto/topos/topo1", "Este mensaje ha sido enviado 5 segundos luego");
  });
}

void loop()
{
  client.loop();
}
//void shoot({enviarmensaje}) 

