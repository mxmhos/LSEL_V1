/*
* Librerias MQTT
* Libreria MQTT basada en MQTTClient.h con la definicion de las funciones simplificadas para comunicarse por MQTT
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "mqtt.h"

/* //Test
#define ADDRESS	"localhost:1883"
#define CLIENTID		"Suscritor"
#define TOPIC			"Prueba"
*/


char* d;
volatile MQTTClient_deliveryToken deliveredtoken;


int mqtt_connect_pub(MQTTClient* client, char *addr, char *id){
	
	int rc; //return code
	
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;//Inicializa opciones de la conexion por defecto
	conn_opts.keepAliveInterval = 20; // Tiempo máximo, en segundos, que debe pasar entre comunicaciones
	//conn_opts.cleansession = 1; //Si existe informacion sobre una conexion anterior, se descarta

	MQTTClient_create(client, addr, id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	
	rc = MQTTClient_connect(*client, &conn_opts);

	return rc; //Devuelve 0 si la operacion se ha completado correctamente (MQTTCLIENT_SUCCESS)
	           //En caso contrario devulve el codigo de error
}

int mqtt_connect(MQTTClient* client, char *addr, char *id){
	
	int rc; //return code
	
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;//Inicializa opciones de la conexion por defecto
	conn_opts.keepAliveInterval = 20; // Tiempo máximo, en segundos, que debe pasar entre comunicaciones
	//conn_opts.cleansession = 1; //Si existe informacion sobre una conexion anterior, se descarta
	
	printf("ADDR: %s\t\tID: %s\n", addr, id);
	
	MQTTClient_create(client, addr, id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	//MQTTClient_setCallbacks establece los callbacks (funciones que se pasan como prarametro). Ejecuta un hilo de fondo que controla
	//el estado de la conexion y otros parámetros. Los callbaks que usa son:
	//  connlost  -> si se ha perdido la conexion
	//	msgarrvd  -> si llega un mensaje
	//  delivered -> cuando se entrega un mensaje
	MQTTClient_setCallbacks(*client, NULL, connlost, msgarrvd, delivered);
	rc = MQTTClient_connect(*client, &conn_opts);

	return rc; //Devuelve 0 si la operacion se ha completado correctamente (MQTTCLIENT_SUCCESS)
	           //En caso contrario devulve el codigo de error
}

int mqtt_publicar(MQTTClient client, char *canal, char *msg){
	
	int rc;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	pubmsg.payload = msg;
	pubmsg.payloadlen = (int)strlen(msg);
	pubmsg.qos = QOS; //Quality Of Service, si se quiere garantizar la entrega del mensaje o no
	pubmsg.retained = 0; //El mensaje no debe ser rentenido por el servidor MQTT

	MQTTClient_publishMessage(client, canal, &pubmsg, &token);
	rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
	return rc;
}

void suscribirse (MQTTClient *client, char *canal){MQTTClient_subscribe(*client, canal, QOS);};

void desuscribirse (MQTTClient* client,  char *canal){MQTTClient_unsubscribe(client, canal);}

void desconectar (MQTTClient* client){
	MQTTClient_disconnect(*client, 10000);
    MQTTClient_destroy(client);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	//printf("Ha llegado un mensaje nuevo\n");
    int longitud = message->payloadlen;
    //char mensaje[longitud];
	//printf("Longitud = %d\n", longitud);
	strncpy(msg_rcv, message->payload, longitud);
	strcpy(canal_rcv, topicName);
	
	mqtt_flag = NEW_MSG;
	
	//Se elimina el mensaje
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
   
}

void delivered(void *context, MQTTClient_deliveryToken dt){
	//printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
	mqtt_flag = MSG_DELIVERED;
}

void connlost(void *context, char *cause)
{
    printf("\nConexion perdida\n");
	mqtt_flag = CON_LOST;
}