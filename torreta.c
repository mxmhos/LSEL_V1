
#include "torreta.h"

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------
int InicializaTorreta (TipoTorreta *p_torreta) {
	int pos_ini_x = ((P_MAX+P_MIN)/2); // Posicion inicial en el punto medio -> 90º
	int pos_ini_y = P_MIN; // Posicion inicial en el punto bajo

	// Configuro PWMs
	pinMode(GPIO19, PWM_OUTPUT);
	pinMode(GPIO18, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(RANGO);
	pwmSetClock (DIVISOR);

	//Establece la posicion inicial de la torreta
	p_torreta->posicion.x = pos_ini_x;
	p_torreta->servo_x.inicio = pos_ini_x;
	p_torreta->servo_x.incremento = INC;
	p_torreta->servo_x.minimo = P_MIN;
	p_torreta->servo_x.maximo = P_MAX;
	
	p_torreta->posicion.y = pos_ini_y;
	p_torreta->servo_y.inicio = pos_ini_y;
	p_torreta->servo_y.incremento = INC;
	p_torreta->servo_y.minimo = P_MIN;
	p_torreta->servo_y.maximo = P_MAX;

	pwmWrite(GPIO18, p_torreta->posicion.x);
	pwmWrite(GPIO19, p_torreta->posicion.y);

	return 0;	
}

//------------------------------------------------------
// FUNCIONES DE THREAD DE MOVIMIENTO
//------------------------------------------------------
void ComprueboTorreta(TipoTorreta *p_torreta) {
	int y_next = 0;
	int x_next = 0;

	if (next_move.y > p_torreta->posicion.y) {
		y_next = p_torreta->posicion.y + p_torreta->servo_y.incremento;
		if (y_next <= p_torreta->servo_y.maximo)
			p_torreta->posicion.y = y_next;
		else
			p_torreta->posicion.y = p_torreta->servo_y.maximo;
		pwmWrite(GPIO19, p_torreta->posicion.y);
	}	
	else if (next_move.y < p_torreta->posicion.y) {
		y_next = p_torreta->posicion.y - p_torreta->servo_y.incremento;
		if (y_next >= p_torreta->servo_y.minimo)
			p_torreta->posicion.y = y_next;
		else
			p_torreta->posicion.y = p_torreta->servo_y.minimo;
		pwmWrite(GPIO19, p_torreta->posicion.y);
	}

	if (next_move.x > p_torreta->posicion.x) {
		x_next = p_torreta->posicion.x + p_torreta->servo_x.incremento;
		if (x_next <= p_torreta->servo_x.maximo)
			p_torreta->posicion.x = x_next;
		else
			p_torreta->posicion.x = p_torreta->servo_x.maximo;
		pwmWrite(GPIO18, p_torreta->posicion.x);
	}	
	else if (next_move.x < p_torreta->posicion.x) {
		x_next = p_torreta->posicion.x - p_torreta->servo_x.incremento;
		if (x_next >= p_torreta->servo_x.minimo)
			p_torreta->posicion.x = x_next;
		else
			p_torreta->posicion.x = p_torreta->servo_x.minimo;
		pwmWrite(GPIO18, p_torreta->posicion.x);
	}
	//fprintf (stdout, "Fun move PosX:%d PosY:%d nextX:%d nextY:%d\n", p_torreta->posicion.x, p_torreta->posicion.y, next_move.x, next_move.y);
}
