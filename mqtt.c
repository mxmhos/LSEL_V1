#include "mqtt.h"
#include "MQTTClient.h"

#define ADDRESS    "192.168.1.202:1883" // "192.168.1.62:1883" // 
#define CLIENTID    "servos"
#define QOS         1
#define TIMEOUT     10000L

#define TOPIC_estado "SERVO/estado"
#define TOPIC_modo "SERVO/modo"
#define TOPIC_atino "SERVO/atino"
#define TOPIC_posX "MANDO/posicionX"
#define TOPIC_posY "MANDO/posicionY"
#define TOPIC_boton "MANDO/boton"

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
	if (strcmp(topicName, "SERVO/estado")==0) {
		estado = atoi(message->payload);
		printf("Recibo estado: %d\n",estado);
		}
	else if (strcmp(topicName, "SERVO/modo")==0) {
		modo = atoi(message->payload);
		printf("Recibo modo: %d\n",estado);
		}
	else if (strcmp(topicName, "SERVO/atino")==0) {
		if (atoi(message->payload)==1)
			flags_player |= FLAG_START_IMPACTO;
		printf("Recibo atino\n");
		}
	else if (strcmp(topicName, "MANDO/posicionX")==0) {
		next_move.x = atoi(message->payload);
		printf("Recibo next_move.x\n");
		}
   	else if (strcmp(topicName, "MANDO/posicionY")==0) {
		next_move.y = atoi(message->payload);
		printf("Recibo next_move.y\n");
		}
	else if (strcmp(topicName, "MANDO/boton")==0) {
		if (atoi(message->payload)==1)
			flags_player |= FLAG_START_DISPARO;
		printf("Recibo boton\n");
		}
	printf("Pase los flags, con next_move.x = %d, next_move.y = %d\n", next_move.x, next_move.y);

	MQTTClient_free(topicName);
	MQTTClient_freeMessage(message);
	return 1;
}

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
   // printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void connlost(void *context, char *cause)
{
	printf("\nConnexion perdida\n");
	printf("     causa: %s\n", cause);
}

int mqtt_init() {
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;

	MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
		printf("Conexión fallida, código de retorno %d\n", rc);
		exit(EXIT_FAILURE);
	}

	MQTTClient_subscribe(client, TOPIC_estado, QOS);
	MQTTClient_subscribe(client, TOPIC_modo, QOS);
	MQTTClient_subscribe(client, TOPIC_atino, QOS);
	MQTTClient_subscribe(client, TOPIC_posX, QOS);
	MQTTClient_subscribe(client, TOPIC_posY, QOS);
	MQTTClient_subscribe(client, TOPIC_boton, QOS);

	return 0;
}
