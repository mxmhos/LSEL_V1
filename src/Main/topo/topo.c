#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "topo.h"


void crear_topo(topo *topo){
	topo->id = "topo1";
	topo->estado = NO_ASIGNADO;
	topo->acierto = NO;
	topo->t_on = 0;
	topo->t_off = 0;
}

int f_topo(topo *topo){
	int n, m;
	int accion = TOPO_NADA;
	
	if (topo->acierto == SI){
		topo->estado = NO_ASIGNADO;
		topo->acierto = NO;
	}
	
	switch (topo->estado){
		case NO_ASIGNADO:
			n = (rand() % 10)+1; //numero aleatorio entre 1 y 10;
			m = (rand() % 10)+1; //numero aleatorio entre 1 y 10;
			topo->t_on = n;
			topo->t_off = m;
			topo->estado=ESCONDIDO;
		break;
		case FUERA:
			if (topo->t_on != 0){topo->t_on--;}
			else {
				topo->estado=NO_ASIGNADO;
				accion = TOPO_ESCONDERSE;
				}
		break;
		case ESCONDIDO:
			if (topo->t_off != 0){topo->t_off--;}
			else {
				topo->estado=FUERA;
				accion = TOPO_SALIR;
			}
		break;
	}
	
	//printf("[TOPO]%s-%d-%d-%d-%d\n", topo->id, topo->estado, topo->acierto, topo->t_on, topo->t_off);
	
	return accion;
	
}