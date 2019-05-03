#ifndef _MQTT_H_
#define _MQTT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include "MQTTClient.h"

#include "piTankGoLib.h"
#include "torreta.h"

extern int flags_player;
extern int estado;
extern int modo;
extern TipoPosicionTorreta next_move;

// Prototipos de funciones de entrada
int mqtt_init();
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);
void delivered(void *context, MQTTClient_deliveryToken dt);

#endif /* _MQTT_H_ */
