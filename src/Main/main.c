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
#define ADDR			"192.168.1.129"		//"172.16.2.4"
#define ID					"Control"

//Canales MQTT
#define TOPO						"TOPO/topo"  // Publico en el canal del topo + su ID -> TOPO/topo1, TOPO/topo2...
#define TOPO_SENSOR   	"TOPO/sonido"
#define CONTROL				"JUEGO/CONTROL"
#define MANDO_DISPARO	"MANDO/disparo"
#define ANDROID       			"android"
#define MANDO				"MANDO/Numero"

// Mensajes MQQTT que maneja Control
#define MSG_CONTROL_START				"1"
#define MSG_CONTROL_STOP				"2"
#define MSG_CONTROL_MARTILLO		"3"
#define MSG_TOPO_DENTRO					"0"
#define MSG_TOPO_FUERA					"1"

//Definicion de flags
#define F_PLAY					0x01
#define F_STOP					0x02
#define F_DISPARO				0x04
#define TOPO_ATINO			0x08

//Tiempo de actualizacion de la FSM en ms --> tiene que ser MCD del tiempo del topo
#define T_FSM_MS		500
#define T_TOPO_MS 	1000		//1 segundo --> Tiempo que actualiza 

//Puntuacion
#define PTOS_MAX	9

#define MSG_MAX				25
#define MAX_TOPOS 			3
#define DATOS_ANDROID 3 //Publica posicion X e Y de la torreta y el estado
#define DELIMITADOR ' '
#define X 0
#define Y 1
#define ESTADO 2
#define MARTILLO "1"
#define PLAY "2"
#define STOP "3"


/******************************************************************************/

// EstadosFSM
enum fsm {OFF, ON};

//Variables MQTT
MQTTClient cliente;
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];
const char *sub_canal[] = {TOPO_SENSOR, ANDROID}; //Canales a los que estoy suscrito

topo n_topo[MAX_TOPOS]; //array de topos

char dato[DATOS_ANDROID][MSG_MAX]; //Datos que envia Andorid

int flag = 0x00;

int tiempo_topo = 0;
int topos_activos = 0;
int puntuacion = 0;

/******************************************************************************/

void analizar_msg(){

	int j = 0;
	int n = 0;
	char c_aux;
	char s_aux[MSG_MAX] = "";
	
	printf("Mensaje nuevo\n");
	printf("\tCanal: %s\n", canal_rcv);
	printf("\tMensaje: %s\n", msg_rcv);
	
	//Mensaje del mando Android
	if( !strcmp(canal_rcv, ANDROID) ){
		for(int i = 0; i < strlen(msg_rcv); i++){
			c_aux = msg_rcv[i];
			if (c_aux != DELIMITADOR){
				sprintf(s_aux, "%s%c", s_aux, c_aux);
			}
			else{
				strcpy(&dato[j][0],s_aux);
				sprintf(s_aux, "%s", ""); //limpiamos la cadena
				j++;
			}
		}
		strcpy(&dato[j][0],s_aux);
		
		if( !strcmp(&dato[ESTADO][0],PLAY) ){flag |= F_PLAY;}
		if( !strcmp(&dato[ESTADO][0],STOP) ){flag |= F_STOP;}
	}
	
	//Mensaje de un topo
	else if( (!strcmp(canal_rcv, TOPO_SENSOR)) && ((flag & F_PLAY) == F_PLAY) ){ //Si no estamos jugando, se ignora
		n = atoi(msg_rcv); //Se envia el identificador del topo al que ha dado
		if( n>0 && n<MAX_TOPOS ){
			n_topo[n-1].acierto = SI;
			flag |= TOPO_ATINO;
			printf("[TOPO%d] Has atinado!\n", n);
		}
	}
	
	sprintf(canal_rcv, " "); //Limpiamos cadenas
	sprintf(msg_rcv, " ");
	
	mqtt_flag &= !NEW_MSG;

}

static int espera (fsm_t* this) {
	printf("Esperando partida\n");
	int start = 0;
	if (mqtt_flag == NEW_MSG){analizar_msg();}
	if ((flag & F_PLAY) == F_PLAY){start = 1;}
	return start;
}
static int jugando (fsm_t* this) {
	
	int stop = 0;
	int accion = TOPO_NADA;
	char canal_topo[MSG_MAX];
	//char aux[MSG_MAX];
	
	//printf("Jugando\n");
	
	if (mqtt_flag == NEW_MSG){analizar_msg();}
	
	if (topos_activos == 0){printf("ERROR: Ningun topo activo\n");}
	else{
		
		//Topos
		if (tiempo_topo == T_TOPO_MS){
			
			for(int i=0; i < MAX_TOPOS; i++){
				if (n_topo[i].id != 0){ // Para los topos activos
					accion = f_topo(&n_topo[i]);
					sprintf(canal_topo, "%s%c", TOPO, n_topo[i].id);
					if (accion == TOPO_SALIR){ //Le ordena salir
						mqtt_publicar(cliente, canal_topo, MSG_TOPO_FUERA);
					}else if (accion == TOPO_ESCONDERSE){ //Le ordena esconderse
						mqtt_publicar(cliente, canal_topo, MSG_TOPO_DENTRO);
					}
				}
			}
			tiempo_topo = 0;
		}else tiempo_topo += T_FSM_MS;
		
		//Mando -> Ahora ya no publico las vidas
		/*if ((flag & TOPO_ATINO) == TOPO_ATINO){
			
			puntuacion--;
			printf("Qedan %d topos vivos\n",puntuacion);
			sprintf(aux, "%d", puntuacion);
			mqtt_publicar(cliente, MANDO, aux);
			flag &= ~TOPO_ATINO;
			
		}*/
		
	}
	
	if ((flag & F_STOP) == F_STOP){stop = 1;}
	return stop;
}

static void inicio (fsm_t* this) {
	// Empieza la partida
	printf("Partida iniciada\n");
	mqtt_publicar(cliente, CONTROL, MSG_CONTROL_START);
}
static void fin (fsm_t* this) {
	// Termina la partida
	printf("Partida terminada\n");
	mqtt_publicar(cliente, CONTROL, MSG_CONTROL_STOP);
	mqtt_flag &= !F_PLAY; //Este permanece activo durante toda la partida, al finalizar se borra
	mqtt_flag &= !F_STOP;
}

// FSM
static fsm_trans_t control[] = {
  { OFF, espera, ON, inicio},
  { ON, jugando, OFF, fin},
  {-1, NULL, -1, NULL },
};

int main (){
	struct timeval clk_period = { 0, T_FSM_MS * 1000 };
	struct timeval next_activation;

	fsm_t* my_fsm = fsm_new (control);
	
	//Establecer conexion MQTT con el Broker
	if (mqtt_connect(&cliente, ADDR, ID) != OK){
		printf("Error al establecer la conexion\n");
		exit(0);
	}printf("Conexion MQTT establecida\n");
	
	//Suscripcion a los caneles definidos
	int n_canales = sizeof(sub_canal)/sizeof(*sub_canal);
	for(int i=0; i < n_canales; i++){
		printf("Te has suscrito al canal: %s\n", sub_canal[i]);
		suscribirse(&cliente, (char *)sub_canal[i]);
	}
	
	//Inicializamos topos
	for(int i = 0; i < sizeof(n_topo)/sizeof(*n_topo); i++){crear_topo(&n_topo[i]);}
	
	//Fuerzo al topo1 a estar activo
	topos_activos = 1;
	n_topo[0].id=1; //topo1
	
	gettimeofday (&next_activation, NULL);
	
	while (1) {
		//printf("Bucle principal\n");
		fsm_fire (my_fsm);
		timeval_add (&next_activation, &next_activation, &clk_period);
		delay_until (&next_activation);
	}
	return 1;
}
