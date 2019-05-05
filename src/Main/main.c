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
#define ADDR			"192.168.1.11"
#define ID					"Control"
#define CANALRAIZ	"JUEGO"
#define MICANAL		"JUEGO/CONTROL"
#define MSG_MAX	25

//Definicion de flags
#define INICIO						0x01
#define FIN							0x02
#define PARAR						0x04
#define TOPO_ATINO			0x08

//Subcanales -> Posicion en el array subcanal
#define TOPO					0
#define MANDO				1
#define CONTROL			2
#define DISPLAY				3

//Posicion en la cadena de mensajes a enviar
#define E_TOPO_DENTRO	0
#define E_TOPO_FUERA		1
#define E_ATINO					2

char tecla;
int flag = 0x00;
int segundos = 0;
int puntuacion = 0;

MQTTClient cliente;
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];

typedef enum{Inicio, Juego, Fin} estado;
estado est = Inicio;

topo topo1; //Creo un topo

const char *subcanal[] = {"TOPO", "MANDO/atino", "CONTROL", "DISPLAY"};
const char *txt[] = {"0", "1", "1"};
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
				flag = PARAR;
			break;
			case 'q':
				printf ("Juego terminado\n");
				mqtt_publicar(cliente, CANALRAIZ, "END");
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
	char c[MSG_MAX];
	
	if ((flag &= INICIO) == INICIO){
		printf ("Juego iniciado\n");
		canal(c, CANALRAIZ, subcanal[CONTROL] );
		mqtt_publicar(cliente, c, "START");
		//flag &= !(INICIO);
		flag = 0x00; //Reseto flags para eliminar cualquier otro flag que se haya acivado
		segundos = 0;
		est = Juego;
	}
}

void analizar_msg(char *destino){
	
	int n_canales = sizeof(subcanal)/sizeof(*subcanal);
	char c[MSG_MAX];
	int longitud;
	
	printf("Mensaje nuevo\n");
	printf("\tCanal: %s\n", canal_rcv);
	printf("\tMensaje: %s\n",msg_rcv);
	for ( int i =0; i < n_canales; i++){
		canal(c, CANALRAIZ, subcanal[i] ); //Busco en todos los subcanales
		longitud = strlen(c);
		if( !strncmp(destino, c, longitud) ){
			switch(i){
				case CONTROL: //Si es un mensaje a control (De momento, /JUEGO/CONTROL/TOPO)
					topo1.acierto = SI;
					flag |= TOPO_ATINO;
					printf("[TOPO] Has atinado!\n");
					printf("[TOPO]%s-%d-%d-%d-%d\n", topo1.id, topo1.estado, topo1.acierto, topo1.t_on, topo1.t_off);
				break;
			}
		}
	}
	
	mqtt_flag &= !NEW_MSG;
	
}

void f_juego(){
	char c[MSG_MAX];
	int accion = TOPO_NADA;
	
	if ((flag & PARAR) == PARAR){
		est = Inicio;
		flag &= ~PARAR;
	}else{
	
		if (mqtt_flag == NEW_MSG){analizar_msg(canal_rcv);}
		
		//Topo
		if (segundos == 1){
			accion = f_topo(&topo1);
			if (accion == TOPO_SALIR){
				canal(c, CANALRAIZ, subcanal[TOPO] );
				mqtt_publicar(cliente, c, (char *)txt[E_TOPO_FUERA]);
			}else if (accion == TOPO_ESCONDERSE){
				canal(c, CANALRAIZ, subcanal[TOPO] );
				mqtt_publicar(cliente, c, (char *)txt[E_TOPO_DENTRO]);
			}
			
		}
		
		//Mando
		if ((flag & TOPO_ATINO) == TOPO_ATINO){
			printf("+++++++++++++++++++\n");
			canal(c, CANALRAIZ, subcanal[MANDO] );
			mqtt_publicar(cliente, c, (char *)txt[E_ATINO]);
			//mqtt_publicar(cliente, c, "1");
			flag &= ~TOPO_ATINO;
		}
	
		segundos = 0;
	}
}

void f_fin(){};



int main(void){
	
	int n_canales = sizeof(subcanal)/sizeof(*subcanal);
	char c[MSG_MAX];
	
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
	for ( int i =0; i < n_canales; i++){
		canal(c, CANALRAIZ, subcanal[i] );
		//canal(c, c, "#" ); // '#' en MQTT sirve para suscribirse a todos los subcanales
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
		//printf("Estado: %d\n", est);
		usleep(500*1000); //500 ms
			
	}
	
	return 0;
	
}