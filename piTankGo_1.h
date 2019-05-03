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
#include <softTone.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.h"
#include "torreta.h"
#include "cliente.h"
#include "mqtt.h"
#include "piTankGoLib.h"

// Posibles estados de las FSMs
enum fsm_state {
	WAIT_START,
	WAIT_NEXT,
	WAIT_END,
	GAME_END,
};


//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraSistema (TipoPlayer *p_sistema);
int InicializaSistema ();
int InicializaTorreta (TipoTorreta *p_torreta);

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------
PI_THREAD (thread_torreta);

#endif /* _PITANKGO_1_H_ */
