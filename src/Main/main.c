#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "MQTT/mqtt.h"

#define QOS				1
#define TIMEOUT		10000L
#define ADDR			"192.168.1.11"
#define ID					"Control"
#define CANALRAIZ	"JUEGO"
#define MICANAL		"JUEGO/CONTROL"
#define MSG_MAX	25

//Definicion de flags
#define INICIO						0x01
#define FIN							0x02
#define JUEGO_INICIADO	0x04
#define JUEGO_PARADO		0x08
#define MSG_NUEVO			0x10

//Topo
#define ESCONDIDO	0
#define FUERA				1

//Subcanales -> Posicion en el array subcanal
#define TOPO			0
#define TORRETA	1
#define SONIDO		2
#define DISPLAY		3

char tecla;
int flag = 0x00;
int segundos = 0;
MQTTClient cliente;

typedef enum{Inicio, Juego, Fin} estado;
estado est = Inicio;

int topo = ESCONDIDO;
const char *subcanal[] = {"TOPO", "TORRETA", "SONIDO", "DISPLAY"};

void canal(char *resultado, char *cadena1, const char *cadena2){sprintf(resultado,"%s/%s", cadena1, cadena2);}

void *teclado(void *arg) {
	
	printf("Whac a Mole!\n");
	printf("Menu inicio:\n");
	printf("\t-Presione la tecla 's' para iniciar una partida\n");
	printf("\t-Presione la tecla 'p' para salir de la partida\n");
	printf("\t-Presione la tecla 'q' para salir del juego\n");
	
	while (tecla != 'q'){
		
		scanf("%c", &tecla);
		
		switch(tecla){
			case 's':
				flag = INICIO;
			break;
			case 'p':
				printf ("Fin de la partida\n");
				mqtt_publicar(cliente, CANALRAIZ, "STOP");
			break;
			case 'q':
				printf ("Juego terminado\n");
				mqtt_publicar(cliente, CANALRAIZ, "END");
			break;
		}
	}
	return 0;
}

void *temporizador(void *arg) {
	sleep(1);
	if (est == Jugando) {segundos++}
	else segundos = 0;
}

void f_inicio(){
	if ((flag &= INICIO) == INICIO){
		printf ("Juego iniciado\n");
		mqtt_publicar(cliente, CANALRAIZ, "START");
		//flag &= !(INICIO);
		flag = 0x00; //Reseto flags para eliminar cualquier otro flag que se haya acivado
		est = Juego;
	}
}

void f_juego(){
	f_topo();
	
}

void f_fin(){};



int main(void){
	
	int n_canales = sizeof(subcanal)/sizeof(*subcanal);
	char c[MSG_MAX];
	
	pthread_t id_teclado;
    pthread_create(&id_teclado, NULL, teclado, NULL); 
	
	pthread_t id_temporizador;
    pthread_create(&id_temporizador, NULL, temporizador, NULL); 
	
	sleep(1);
	printf("Llego hasta aqui\n");
	if (mqtt_connect(&cliente, ADDR, ID) != OK){
		printf("Error al establecer la conexion\n");
		exit(0);
	}
	
	sleep(1);
	
	printf("Te has suscrito a %d canales:\n", n_canales);
	for ( int i =0; i < n_canales; i++){
		canal(c, CANALRAIZ, subcanal[i] );
		suscribirse(&cliente, c);
		printf("\t%s\n", c);
	}
	
	//printf("Llego hasta aqui\n");
	
	while(tecla != 'q'){
		switch(est){
			case Inicio:
			//¿Hacemos comprobacion de que existe, al menos, una torreta y un topo antes de iniciar? --> Puede ser una mejora para el porx hito
				f_inicio();
			break;
			case Juego:
				f_juego();
			break;
			case Fin:
				f_fin();
			break;
		}
			
	}
	
	return 0;
	
}