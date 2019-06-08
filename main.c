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
#define ADDR			"192.168.1.102"		//"172.16.2.4"
#define ID					"Control"
#define MSG_MAX	25

//Canales MQTT suscrito
#define SUSCRITO_ACIERTO_1		"topo1/acierto" //Publica 1 si le han dado al topo
#define SUSCRITO_ACIERTO_2		"topo2/acierto"
#define SUSCRITO_ACIERTO_3		"topo3/acierto"
#define SUSCRITO_START				"MANDO/start" //Empieza la partida
#define SUSCRITO_STOP				"MANDO/stop" //Termina la partida

//Canales MQTT publica
#define CANAL_SERVO_1			"topo1/posicion" //Publica 1 si el topo sale y 0 si se esconde
#define CANAL_SERVO_2			"topo2/posicion"
#define CANAL_SERVO_3			"topo3/posicion"
#define CANAL_ESTADO				"estado"
#define CANAL_PUNTUACION		"puntuacion"
#define CANAL_TIEMPO				"tiempo"

// Mensajes MQQTT que maneja Control
#define MSG_CONTROL_PLAY				"1"
#define MSG_CONTROL_STOP				"2"
#define MSG_CONTROL_WIN					"3"
#define MSG_CONTROL_LOSE				"4"
#define MSG_CONTROL_MARTILLO		"3"
#define MSG_TOPO_DENTRO					"0"
#define MSG_TOPO_FUERA					"1"

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
#define TIEMPO_JUEGO 		99 //Tiempo de juego 99 segundos


/******************************************************************************/

// EstadosFSM
enum fsm {OFF, ON};

//Variables MQTT
MQTTClient cliente;
char mqtt_flag;
char msg_rcv[MSG_MAX];
char canal_rcv[MSG_MAX];
const char *sub_canal[] = {SUSCRITO_ACIERTO_1, SUSCRITO_ACIERTO_2, SUSCRITO_ACIERTO_3, SUSCRITO_START, SUSCRITO_STOP}; //Canales a los que estoy suscrito

const char servo[3][MSG_MAX] = {CANAL_SERVO_1, CANAL_SERVO_2, CANAL_SERVO_3};

topo n_topo[MAX_TOPOS]; //array de topos

int flag = 0x00;

int cuentaatras = 0; //Tiempo de juego
int segundo = 0; // segundo
int puntuacion = 0;

/******************************************************************************/

void analizar_msg(){
	int id = 0;
	//char aux[MSG_MAX] = "";
	
	printf("Mensaje nuevo\n");
	printf("\tCanal: %s\n", canal_rcv);
	printf("\tMensaje: %s\n", msg_rcv);
	
	//Mensaje del mando Android
	if( !strcmp(canal_rcv, SUSCRITO_START) ){flag |= F_PLAY;}
	else if( !strcmp(canal_rcv, SUSCRITO_STOP) ){flag |= F_STOP;}
	
	//Mensaje de un topo
	else if ((flag & F_PLAY) == F_PLAY){ //Si no estamos jugando, se ignora
		if ( !strcmp(canal_rcv, SUSCRITO_ACIERTO_1) ){ id = 1;}
		else if( !strcmp(canal_rcv, SUSCRITO_ACIERTO_2) ){ id = 2;}
		else if( !strcmp(canal_rcv, SUSCRITO_ACIERTO_3) ){ id = 3;}
			
		if( id>0 && id<MAX_TOPOS ){
			n_topo[id-1].acierto = SI;
			flag |= TOPO_ATINO;
			printf("[TOPO%d] Has atinado!\n", id);
			mqtt_publicar(cliente, (char *)servo[id-1], MSG_TOPO_DENTRO);
		}
	}
	
	sprintf(canal_rcv, " "); //Limpiamos cadenas
	sprintf(msg_rcv, " ");
	
	mqtt_flag &= !NEW_MSG;

}

int actualizo_puntuacion(){
	
	int fin_vidas = 0;
	
	char aux[MSG_MAX];
	
	if (puntuacion > 0){
			puntuacion--;
			printf("Qedan %d topos vivos\n",puntuacion);
			sprintf(aux, "%d", puntuacion);
			//mqtt_publicar(cliente, CANAL_PUNTUACION, aux);
			flag &= ~TOPO_ATINO;
		}
		else{
			flag |= F_WIN;
			fin_vidas = 1; //Se han matado a todos los topos
		}
	return fin_vidas;
}

void actualizo_topos(){
	int accion = TOPO_NADA;
	char aux[MSG_MAX];
	int posicion_topo = 0;
	
	for(int i=0; i < MAX_TOPOS; i++){
		
		accion = f_topo(&n_topo[i]);	// Comprueba los tiempos tiempos de los topos y devuelve la accion a tomar:
													// 		- TOPO_SALIR si se le ha acabado el tiempo de escondido
													// 		- TOPO_ESCONDERSE si se le ha acabado el tiempo que tiene que estar fuera
		
		if (accion == TOPO_SALIR){ //Le ordena salir
			posicion_topo = 2;
		}
		else if (accion == TOPO_ESCONDERSE){ //Le ordena esconderse
			posicion_topo = 1;
		}
		
		if (posicion_topo) { // Para los topos que se les ha agotado el tiempo, se le publica la accion que deben tomar (esconderde o salir)
			sprintf(aux, "%d", posicion_topo-1);
			mqtt_publicar(cliente, (char *)servo[i], aux);
			/* printf("Posicion: %d\n", posicion_topo);	
			printf("Topo: %d \tAccion: %s\n", i, aux); */
		}
		posicion_topo = 0;
	}
}

void debbug(){
	printf("Vidas %d\tTiempo... %d\n",puntuacion, cuentaatras);
	
	 printf("TOPO\t 1\t 2\t 3\t\n");
	printf("Estado\t %d\t %d\t %d\t\n",n_topo[0].estado, n_topo[1].estado, n_topo[2].estado);
	printf("Acierto\t %d\t %d\t %d\t\n",n_topo[0].acierto, n_topo[1].acierto, n_topo[2].acierto);
	printf("T_on\t %d\t %d\t %d\t\n",n_topo[0].t_on, n_topo[1].t_on, n_topo[2].t_on);
	printf("T_off\t %d\t %d\t %d\t\n",n_topo[0].t_off, n_topo[1].t_off, n_topo[2].t_off);
	
	printf("------------------------------------------------------------------------------\n");
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
	char aux[MSG_MAX];
	
	segundo += T_FSM_MS;
	
	if (mqtt_flag == NEW_MSG){analizar_msg();} // Si ha llegado un mensaje, se analiza
	
	if ((flag & TOPO_ATINO) == TOPO_ATINO){stop = actualizo_puntuacion();} // Si ha muerto un topo, se actualiza la puntuacion
	if (segundo == T_SEGUNDO_MS && !stop){ //Cada segundo se actualiza el estado del juego, la cuenta atras, los topos...
		sprintf(aux, "%d", cuentaatras);
		mqtt_publicar(cliente, CANAL_TIEMPO, aux);
		if (cuentaatras == 0){
			flag |= F_LOSE;
			stop = 1;
		}
		else{
			actualizo_topos();
			cuentaatras--;
		}
		segundo = 0;
		
		debbug(); // Mensajes a mostrar para depuracion
	}
	
	if ((flag & F_STOP) == F_STOP){stop = 1;}
	return stop;
}

static void inicio (fsm_t* this) {
	// Empieza la partida
	char aux[MSG_MAX];
	
	printf("Partida iniciada\n");
	cuentaatras = TIEMPO_JUEGO; //Reinicio el tiempo y las vidas
	puntuacion = PTOS_MAX;
	sprintf(aux, "%d", puntuacion);
	mqtt_publicar(cliente, CANAL_PUNTUACION, aux);
	mqtt_publicar(cliente, CANAL_ESTADO, MSG_CONTROL_PLAY);
}
static void fin (fsm_t* this) { // Termina la partida
	
	printf("Partida terminada\n");
	
	if ((flag & F_LOSE) == F_LOSE) mqtt_publicar(cliente, CANAL_ESTADO, MSG_CONTROL_LOSE);
	else if ((flag & F_WIN) == F_WIN) mqtt_publicar(cliente, CANAL_ESTADO, MSG_CONTROL_WIN);
	else mqtt_publicar(cliente, CANAL_ESTADO, MSG_CONTROL_STOP);
	
	for(int i=0; i < MAX_TOPOS; i++){mqtt_publicar(cliente, (char *)servo[i], MSG_TOPO_DENTRO);}
	
	flag = 0x00; // limpio todos los flags
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
	crear_topo(&n_topo[0]); //topo1
	crear_topo(&n_topo[1]); //topo2
	crear_topo(&n_topo[2]); //topo3
	n_topo[0].id=1;
	n_topo[1].id=2;
	n_topo[2].id=3;
	
	gettimeofday (&next_activation, NULL);
	
	while (1) {
		//printf("Bucle principal\n");
		fsm_fire (my_fsm);
		timeval_add (&next_activation, &next_activation, &clk_period);
		delay_until (&next_activation);
	}
	return 1;
}
