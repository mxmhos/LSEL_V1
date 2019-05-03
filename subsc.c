//gcc subsc.c -o subsc -lpaho-mqtt3c -lwiringPi
//./subsc
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include "MQTTClient.h"

#define ADDRESS     "192.168.43.148:1883"
#define CLIENTID    "topo1s"
#define TOPIC       "topos"
#define PAYLOAD     "altosub"
#define QOS         1
#define TIMEOUT     10000L

char* d;

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
   // printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf("Mensaje recibido\n");
    printf("     tema: %s\n", topicName);
    printf("   mensaje: ");

    payloadptr = message->payload;
d=payloadptr ;    
for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
       
    }
    putchar('\n');
    
 
    MQTTClient_free(topicName);
     printf("d = %s\n",d);
      if(!strcmp(d, "2")){
	pinMode(29, OUTPUT);
    system("mplayer starwras.mp3");
	int i = 0;
	for (i = 0;i<10;i++){
		digitalWrite(29, HIGH);
		delay(500);
		digitalWrite(29, LOW);
		delay(500);
	}
    return 0 ;
}
    return 1;
   
}

void connlost(void *context, char *cause)
{
    printf("\nConnexion perdida\n");
    printf("     causa: %s\n", cause);
}

int main(int argc, char* argv[])
{   wiringPiSetup();
	
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;
    
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
   
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Conexión fallida, código de retorno %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribirse al tema %s\npor el cliente %s usando QoS%d\n\n"
           "Presione Q<Enter> para salir\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do 
    { 
        ch = getchar();
    
       
    } while(ch!='Q' && ch != 'q');
    
    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
