#ifndef _MQTT_H_
#define _MQTT_H_

#include <MQTTClient.h>

#define QOS         1
#define OK			0
#define TIMEOUT     10000L

#define NEW_MSG				0x01
#define CON_LOST				0x02
#define MSG_DELIVERED	0x04

#define MSG_MAX				25


//Variables globales
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];

// Funciones
int mqtt_connect_pub(MQTTClient* client, char *addr, char *id); //Conectarse solo en modo publicador

int mqtt_connect(MQTTClient* client, char *addr, char *id); //Conectarse en modo publicador y suscritor

int mqtt_publicar(MQTTClient client, char *canal, char *msg); //Publica un mensaje en el canal indicado

void connlost(void *context, char *cause);

void delivered(void *context, MQTTClient_deliveryToken dt);

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

void suscribirse (MQTTClient *client, char *canal);

void desuscribirse (MQTTClient* client,  char *canal);

void desconectar (MQTTClient* client);

#endif