#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS "localhost"
#define QOS 1
#define TOPIC "disparo"
#define CLIENTID "sonido_disparo"
int main(void)
{
        int rc;
        int ch = 0;
        MQTTClient client;
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

        MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;

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
                if (ch)
                {
                        system("mplayer starwras.mp3");
                }

        } while(ch!='Q' && ch != 'q');

}
