#ifndef _TOPO_H_
#define _TOPO_H_

#define SI		1
#define NO	0

#define FUERA					1
#define ESCONDIDO		0
#define NO_ASIGNADO	2

#define TOPO_ESCONDERSE 0
#define TOPO_SALIR 1
#define TOPO_NADA 2

typedef struct{
	char *id;
	int estado;
	int acierto;
	int t_on;
	int t_off;
}topo;

void crear_topo(topo *topo);

int f_topo(topo *topo);
	
	
#endif