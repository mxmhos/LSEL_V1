/*
 * piTankGo_1.h
 *
 *  Created on: 11 de enero de 2019
 *      Author: FFM
 */

#ifndef _PITANKGO_1_H_
#define _PITANKGO_1_H_

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "tmr.h"
#include "torreta.h"
#include "cliente.h"
#include "piTankGoLib.h"

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraSistema ();
int InicializaSistema ();
int InicializaTorreta (TipoTorreta *p_torreta);
void timer_laser_duracion_disparo_isr (union sigval value);

#endif /* _PITANKGO_1_H_ */
