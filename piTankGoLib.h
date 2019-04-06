
#ifndef _PITANKGOLIB_H_
#define _PITANKGOLIB_H_

#include <stdio.h>

#include "fsm.h"
#include "tmr.h"
#include "torreta.h"

#define CLK_MS 10
#define CLK_THREAD 10000
#define CLK_TRR 10

// DIRECCION DE SERVER
#define host "192.168.1.202" // "172.16.2.3" // // "172.16.2.4"
#define puerto	8080

// VALORES DE CONFIGURACION SERVER
#define SYSCLK 19200	//19.2MHz = 19200kHz
#define DIVISOR 192 	//100kHz
#define RANGO	2000	// 0 = OFF, T=2000=100%
#define T 20		// T=20ms -> f=50Hz

//Pin GPIO salida PWM
#define GPIO18 18	//eje y
#define GPIO19 19	//eje x

//SERVOMOTOR
#define INC 1		//Incremento cada vez que se pulsa una tecla
#define P_MAX 200	//Pulso maximo 2ms -> p_max=RANGO*2/T=200 -> Torreta en 180º -> Podemos usar 100 intervalos del total de RANGO
#define P_MIN 100	//Sin pulso, PWM off -> Torreta en 0º


// ATENCION: Valores a modificar por el alumno
// INTERVALO DE GUARDA ANTI-REBOTES
#define	DEBOUNCE_TIME	0
// DURACION DISPARO IR
#define SHOOTING_PERIOD 0

// CLAVES PARA MUTEX
// ATENCION: Valores a modificar por el alumno
#define	SYSTEM_FLAGS_KEY	0
#define	PLAYER_FLAGS_KEY	0
#define	STD_IO_BUFFER_KEY	0

// Distribucion de pines GPIO empleada para el teclado
// ATENCION: Valores a modificar por el alumno
#define	TECLADO_ROW_1	0
#define	TECLADO_ROW_2	0
#define	TECLADO_ROW_3	0
#define	TECLADO_ROW_4	0

#define	TECLADO_COL_1	0
#define	TECLADO_COL_2	0
#define	TECLADO_COL_3	0
#define	TECLADO_COL_4	0

//#define JOY_PIN_UP	0
//#define JOY_PIN_DOWN	0
//#define JOY_PIN_LEFT	0
//#define JOY_PIN_RIGHT	0
//#define JOY_PIN_CENTER 0

// Distribucion de pines GPIO empleada para el enlace IR
// ATENCION: Valores a modificar por el alumno
#define	IR_TX_PIN		0
#define	IR_RX_PIN		0

// Distribucion de pines GPIO empleada para la reproducción de efectos
// ATENCION: Valores a modificar por el alumno

#define PLAYER_PWM_PIN 24 	//GPIO18
#define NO_SONAR 0 		//frecuencia de 0Hz

// Distribucion de pines GPIO empleada para el control de los servos
// ATENCION: Valores a modificar por el alumno
#define	SERVO_VERTICAL_PIN		0
#define	SERVO_HORIZONTAL_PIN	0


// FLAGS FSM CONTROL DE JUEGO Y TORRETA
// ATENCION: Valores a modificar por el alumno
#define FLAG_SYSTEM_START 		0x001
#define FLAG_JOYSTICK_UP 		0x002
#define FLAG_JOYSTICK_DOWN 		0x004
#define FLAG_JOYSTICK_LEFT		0x008
#define FLAG_JOYSTICK_RIGHT		0x010
#define FLAG_TRIGGER_BUTTON		0x020
#define FLAG_SHOOT_TIMEOUT		0x040
#define FLAG_TARGET_DONE		0x080
#define FLAG_SYSTEM_END			0x100
#define FLAG_MOVE			0x200

// FLAGS FSM REPRODUCCION DE EFECTOS DE SONIDO
// ATENCION: Valores a modificar por el alumno
#define FLAG_START_DISPARO 		0x01
#define FLAG_START_IMPACTO 		0x02
#define FLAG_PLAYER_END			0x04
#define FLAG_NOTA_TIMEOUT		0x08

enum interruption_sources {
	TARGET_ISR,
	TECLADO_FILA_1,
	TECLADO_FILA_2,
	TECLADO_FILA_3,
	TECLADO_FILA_4,
	//JOYSTICK_UP_ISR,
	//JOYSTICK_DOWN_ISR,
	//JOYSTICK_LEFT_ISR,
	//JOYSTICK_RIGHT_ISR,
	//JOYSTICK_CENTER_ISR
};


typedef struct {
	int x; // Coordenada x correspondiente a la posicion del servo horizontal
	int y; // Coordenada y correspondiente a la posicion del servo vertical
} TipoPosicionTorreta2;

extern int flags_system;
extern int flags_player;
extern int flags_juego;
extern TipoPosicionTorreta2 next_move;

extern int frecuenciaDespacito[];
extern int tiempoDespacito[];
extern int frecuenciaGOT[];
extern int tiempoGOT[];
extern int frecuenciaTetris[];
extern int tiempoTetris[];
extern int frecuenciaStarwars[];
extern int tiempoStarwars[];

extern int frecuenciasDisparo[];
extern int tiemposDisparo[];
extern int frecuenciasImpacto[];
extern int tiemposImpacto[];

#endif /* _PITANKGOLIB_H_ */
