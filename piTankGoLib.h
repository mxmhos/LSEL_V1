
#ifndef _PITANKGOLIB_H_
#define _PITANKGOLIB_H_

#include <stdio.h>

#include "tmr.h"
#include "torreta.h"

#define CLK_MAIN 10
#define DURACION_LASER 1500

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

// Distribucion de pines GPIO empleada para el enlace LASER
#define	LASER_PIN		4

// Distribucion de pines GPIO empleada para el control de los servos
#define	SERVO_VERTICAL_PIN	0
#define	SERVO_HORIZONTAL_PIN	0


#endif /* _PITANKGOLIB_H_ */
