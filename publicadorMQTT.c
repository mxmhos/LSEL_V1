#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <MQTTClient.h>
#include "tiempo.h"
#include "fsm.h"
#include "MQTT/mqtt.h"
#include "topo/topo.h"

//Conexion MQTT
#define QOS				1
#define TIMEOUT		10000L
#define ADDR			"192.168.43.27"		//"172.16.2.4"
#define ID					"test"
#define MSG_MAX	25

//Canales MQTT suscrito
#define SUSCRITO_ACIERTO_1		"topo1/acierto" //Publica 1 si le han dado al topo
#define SUSCRITO_ACIERTO_2		"topo2/acierto"
#define SUSCRITO_ACIERTO_3		"topo3/acierto"
#define SUSCRITO_START				"mando/start" //Empieza la partida
#define SUSCRITO_STOP				"mando/stop" //Termina la partida

//Definicion de flags
#define F_PLAY					0x01
#define F_STOP					0x02
#define F_WIN						0x04
#define F_LOSE					0x08
#define F_DISPARO				0x10
#define TOPO_ATINO			0x20

//Tiempo de actualizacion de la FSM en ms --> tiene que ser MCD del tiempo del topo
#define T_FSM_MS				500
#define T_SEGUNDO_MS 	1000		//1 segundo

//Juego
#define PTOS_MAX				8
#define MAX_TOPOS 			3
#define TIEMPO_JUEO 		99 //Tiempo de juego 99 segundos


/******************************************************************************/

MQTTClient cliente;
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];

int main (){
	
	struct timeval clk_period = { 0, T_FSM_MS * 1000 };
	struct timeval next_activation;
	
	if (mqtt_connect(&cliente, ADDR, ID) != OK){
		printf("Error al establecer la conexion\n");
		exit(0);
	}printf("Conexion MQTT establecida\n");
	
	gettimeofday (&next_activation, NULL);
	
	char tecla;
	int salir=1;
	
	printf("Presiona una tecla para simular una accion\n");
	printf("\t 1 = Acierto topo1\n");
	printf("\t 2 = Acierto topo2\n");
	printf("\t 3 = Acierto topo3\n");
	printf("\t 4 = START\n");
	printf("\t 5 = STOP \n");
	printf("\t q = salir \n");
	
	while (salir) {
		scanf("%c", &tecla);
		
		switch(tecla){
			case '1':
				mqtt_publicar(cliente, "topo1/acierto", "1");
			break;
			case '2':
				mqtt_publicar(cliente, "topo2/acierto", "1");
			break;
			case '3':
				mqtt_publicar(cliente, "topo3/acierto", "1");
			break;
			case '4':
				mqtt_publicar(cliente, "mando/start", "1");
			break;
			case '5':
				mqtt_publicar(cliente, "mando/stop", "1");
			break;
			case 'q':
				printf ("Saliendo del programa...\n"); 
				salir = 0;
			break;
		}
	}
	return 1;
}
