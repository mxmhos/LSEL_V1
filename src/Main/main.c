#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <MQTTClient.h>
#include "MQTT/mqtt.h"
#include "topo/topo.h"

#define QOS				1
#define TIMEOUT		10000L
#define ADDR			"172.16.2.4"
#define ID					"Control"
#define CANALRAIZ	"JUEGO"
#define MICANAL		"JUEGO/CONTROL"
#define MSG_MAX	25

//Definicion de flags
#define INICIO						0x01
#define FIN							0x02
#define PARAR						0x04
#define TOPO_ATINO			0x08

//Subcanales
#define TOPO_SERVO		"TOPO/topo1"
#define TOPO_SENSOR	"TOPO/sonido"
#define MANDO				"MANDO/Numero"
#define CONTROL			"JUEGO/CONTROL"

//Subcanales -> Posicion en el array canal de subcripciones
#define SUB_TOPO_SENSOR		0


//Subcanales -> Posicion en el array canal de publicaciones
#define PUB_CONTROL			0
#define PUB_MANDO				1
#define PUB_TOPO_SERVO		2

//Posicion en la cadena de mensajes a publicar
#define MSG_CONTROL_START		"1"
#define MSG_CONTROL_STOP		"0"
#define MSG_CONTROL_FIN			"2"
#define MSG_CONTROL_WIN			"3"
#define MSG_CONTROL_LOSE		"4"
#define MSG_TOPO_DENTRO			"0"
#define MSG_TOPO_FUERA			"1"
#define MSG_ATINO							"1"

//Tiempo espera 500 ms
#define T_500_MS 500000

//Puntuacion
#define PTOS_MAX	9

//Tiempo de duracion de la partida en segundos
#define T_PARTIDA 60

char tecla;
int flag = 0x00;
int segundos = 0;
int puntuacion = 0;
int cuantaatras = T_PARTIDA;

MQTTClient cliente;
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];

int acabar_partida = 0;

typedef enum{Inicio, Preparado, Juego, Fin} estado;
estado est = Inicio;

topo topo1; //Creo un topo

const char *sub_canal[] = {TOPO_SENSOR}; //Canales en los que estoy suscrito
const char *pub_canal[] = {CONTROL, MANDO, TOPO_SERVO}; //Canales en los que publico

void canal(char *resultado, char *cadena1, const char *cadena2){sprintf(resultado,"%s/%s", cadena1, cadena2);}

void menu(){
	
	printf("\t***********************************\n");
	printf("\t             Whac a Mole!          \n");
	printf("\t***********************************\n");
	printf("\n");
	printf("Menu inicio:\n");
	printf("\t-Presione la tecla 's' para iniciar una partida\n");
	printf("\t-Presione la tecla 'p' para salir de la partida\n");
	printf("\t-Presione la tecla 'q' para salir del juego\n");
	printf("\nPulse una tecla: \n");
	
}

void *teclado(void *arg) {
	
	while (tecla != 'q'){
		
		scanf("%c", &tecla);
		
		switch(tecla){
			case 's':
				flag = INICIO;
			break;
			case 'p':
				flag = PARAR;
			break;
			case 'q':
				flag = FIN;
			break;
		}
	}
	return 0;
}

void *temporizador(void *arg) { //Activa un flag cada vez que pasa 1 segundo
	while (tecla != 'q'){
		sleep(1);
		if (est == Juego) {segundos=1;}
		else segundos = 0;
	}
	return 0;
}

void f_inicio(){
	//char c[MSG_MAX];
	
	menu();
	
	//Inicializamos todas las variables
	flag = 0x00;
	segundos = 0;
	puntuacion = PTOS_MAX;
	cuantaatras = T_PARTIDA;
	
	//Todo listo para empezar
	est = Preparado;
	
}

void f_preparado(){
	
	if ((flag &= INICIO) == INICIO){
		printf ("Juego iniciado\n");
		mqtt_publicar(cliente, CONTROL, MSG_CONTROL_START);
		flag &= ~INICIO;
		est = Juego;
	}
}

void analizar_msg(char *destino){
	
	printf("Mensaje nuevo\n");
	printf("\tCanal: %s\n", canal_rcv);
	printf("\tMensaje: %s\n",msg_rcv);
	
	if( !strcmp(canal_rcv, TOPO_SENSOR) ){
		flag |= TOPO_ATINO;
		printf("[TOPO] Has atinado!\n");
	}
	
	mqtt_flag &= !NEW_MSG;
	
}

void f_juego(){
	//char c[MSG_MAX];
	int accion = TOPO_NADA;
	char aux[MSG_MAX];
	
	if ((flag & PARAR) == PARAR){
		
		mqtt_publicar(cliente, CONTROL, MSG_CONTROL_STOP);
		
		est = Inicio;
		flag &= ~PARAR;
		
	}else{
	
		if (mqtt_flag == NEW_MSG){analizar_msg(canal_rcv);}
		
		//Topo
		if (segundos == 1){
			accion = f_topo(&topo1);
			if (accion == TOPO_SALIR){
				mqtt_publicar(cliente, TOPO_SERVO, MSG_TOPO_FUERA);
			}else if (accion == TOPO_ESCONDERSE){
				mqtt_publicar(cliente, TOPO_SERVO, MSG_TOPO_DENTRO);
			}
			
			cuantaatras--;
			printf("Tiempo restante: %d\n", cuantaatras);
			
		}
		
		//Mando
		if ((flag & TOPO_ATINO) == TOPO_ATINO){
			
			puntuacion--;
			topo1.acierto = SI;
			
			printf("Qedan %d topos vivos\n",puntuacion);
			sprintf(aux, "%d", puntuacion);
			mqtt_publicar(cliente, MANDO, aux);
			flag &= ~TOPO_ATINO;
			
		}
		
		// Control de la partida
		if ((puntuacion == 0) && (cuantaatras > 0)){
			printf("\t***********************************\n");
			printf("\t             YOU WIN          \n");
			printf("\t***********************************\n\n");
			mqtt_publicar(cliente, CONTROL, MSG_CONTROL_WIN);
			est = Inicio;
		}else if ((puntuacion > 0) && (cuantaatras == 0)){
			printf("\t***********************************\n");
			printf("\t             YOU LOSE!          \n");
			printf("\t***********************************\n\n");
			mqtt_publicar(cliente, CONTROL, MSG_CONTROL_LOSE);
			est = Inicio;
		}
		
		segundos = 0;
	}
}

void f_fin(){
	//char c[MSG_MAX];
	//canal(c, CANALRAIZ, pub_canal[PUB_CONTROL] );
	mqtt_publicar(cliente, CONTROL, MSG_CONTROL_FIN);
	mqtt_publicar(cliente, TOPO_SERVO, MSG_TOPO_DENTRO);
	acabar_partida = 1;
};



int main(void){
	
	int n_canales = sizeof(sub_canal)/sizeof(*sub_canal);
	//char c[MSG_MAX];
	
	pthread_t id_teclado;
    pthread_create(&id_teclado, NULL, teclado, NULL); 
	
	pthread_t id_temporizador;
    pthread_create(&id_temporizador, NULL, temporizador, NULL); 
	
	crear_topo(&topo1);
	
	if (mqtt_connect(&cliente, ADDR, ID) != OK){
		printf("Error al establecer la conexion\n");
		exit(0);
	}
	
	printf("Te has suscrito a %d canales:\n", n_canales);
	suscribirse(&cliente, "sonido");
	printf("\t%s\n", "sonido");
	
	while(!acabar_partida){
		switch(est){
			case Inicio:
				f_inicio();
			break;
			case Preparado:
				f_preparado();
			break;
			case Juego:
				f_juego();
			break;
			case Fin:
				f_fin();
			break;
		}
		//printf("Estado: %d\n", est);
		usleep(T_500_MS); //esperamos 500 ms entre cada transicion
			
	}
	
	return 0;
	
}
