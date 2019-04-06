
#include "torreta.h"

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaTorreta (TipoTorreta *p_torreta) {
	// A completar por el alumno...
	int pos_ini = ((P_MAX-P_MIN)/2)+P_MIN; // Posicion inicial en el punto medio -> 90º

	pinMode(GPIO19, PWM_OUTPUT);
	pinMode(GPIO18, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(RANGO);
	pwmSetClock (DIVISOR);
	/*
			Establece la posicion inicial de la torreta
		*/
	p_torreta->posicion.x = pos_ini;
	p_torreta->servo_x.inicio = pos_ini;
	p_torreta->servo_x.incremento = INC;
	p_torreta->servo_x.minimo = P_MIN;
	p_torreta->servo_x.maximo = P_MAX;
	
	p_torreta->posicion.y = pos_ini;
	p_torreta->servo_y.inicio = pos_ini;
	p_torreta->servo_y.incremento = INC;
	p_torreta->servo_y.minimo = P_MIN;
	p_torreta->servo_y.maximo = P_MAX;

	pwmWrite(GPIO19, p_torreta->posicion.x);
	pwmWrite(GPIO18, p_torreta->posicion.y);
	
	
	
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaComienzo (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_SYSTEM_START){
		printf ("Recibida senal START\n");
		result = 1;
	}
	
	return result;
}

int CompruebaMove (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_MOVE){
		fprintf (stdout, "Joystick -> MOVE\n");
		result = 1;
	}

	return result;
}

int CompruebaJoystickUp (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_JOYSTICK_UP){
		fprintf (stdout, "Joystick -> UP\n");
		result = 1;
	}

	return result;
}

int CompruebaJoystickDown (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_JOYSTICK_DOWN){
		fprintf (stdout, "Joystick -> DOWN\n");
		result = 1;
	}

	return result;
}

int CompruebaJoystickLeft (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_JOYSTICK_LEFT){
		fprintf (stdout, "Joystick -> LEFT\n");
		result = 1;
	}

	return result;
}

int CompruebaJoystickRight (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_JOYSTICK_RIGHT){
		fprintf (stdout, "Joystick -> RIGHT\n");
		result = 1;
	}

	return result;
}

int CompruebaTriggerButton (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_TRIGGER_BUTTON){
		fprintf (stdout, "Recibida senal FIRE\n");
		result = 1;
	}
	return result;
}

int CompruebaImpacto (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_TARGET_DONE){
		fprintf (stdout, "Recibida senal TARGET\n");
		result = 1;
	}
	else {
		fprintf (stdout, "NO Recibida senal TARGET, pero salgo igual!\n");
		result = 1;
	}
	return result;
}

int CompruebaTimeOutDisparo (fsm_t* this) {
	int result = 0;
	if (flags_juego & FLAG_SHOOT_TIMEOUT){
		fprintf (stdout, "Recibida senal TIMEOUT\n");
		result = 1;
	}
	else {
		fprintf (stdout, "NO Recibida senal TIMEOUT, pero salgo igual!\n");
		result = 1;
	}
	return result;
}

int CompruebaFinalJuego (fsm_t* this) {
	int result = 0;

	if (flags_juego & FLAG_SYSTEM_END){
		fprintf (stdout, "Recibida senal END\n");
		result = 1;
	}

	return result;
}

int NoComprueboNada (fsm_t* this) {
	fprintf (stdout, "\t\tNo compruebo nada\n");
	return 1;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ComienzaSistema (fsm_t* this) {
	// A completar por el alumno
	// ...
	
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);
	InicializaTorreta(torreta);
	fprintf (stdout, "Iniciando Torreta...\n");
	flags_juego &= ~FLAG_SYSTEM_START; //Limpiamos flag
	fprintf (stdout, "Torreta operativa\n");
}


void FunMueve (fsm_t* this) {
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);

	if (next_move.y>P_MAX)
		torreta->posicion.y = P_MAX;
	else if (next_move.y<P_MIN)
		torreta->posicion.y = P_MIN;
	else
		torreta->posicion.y = next_move.y;
	pwmWrite(GPIO19, torreta->posicion.y);

	if (next_move.x>P_MAX)
		torreta->posicion.x = P_MAX;
	else if (next_move.x<P_MIN)
		torreta->posicion.x = P_MIN;
	else
		torreta->posicion.x = next_move.x;
	pwmWrite(GPIO18, torreta->posicion.x);

	fprintf (stdout, "Fun move PosX:%d PosY:%d nextX:%d nextY:%d\n", torreta->posicion.x, torreta->posicion.y, next_move.x, next_move.y);
	flags_juego &= ~FLAG_MOVE;
}

void MueveTorretaArriba (fsm_t* this) {
	int y_next = 0;
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);	
	fprintf (stdout, "Accion -> Torreta UP\n");
	
	while (torreta->posicion.y<next_move.y) {
		y_next = torreta->posicion.y + torreta->servo_y.incremento;
		
		if (y_next <= torreta->servo_y.maximo) {
			torreta->posicion.y = y_next;
			pwmWrite(GPIO19, torreta->posicion.y);
		}
		else if(torreta->posicion.y < torreta->servo_y.maximo) {
			torreta->posicion.y = torreta->servo_y.maximo;
			pwmWrite(GPIO19, torreta->posicion.y);
			break;
		}
	delay(CLK_TRR);
	}
	flags_juego &= ~FLAG_JOYSTICK_UP;
}

void MueveTorretaAbajo (fsm_t* this) {
	int y_next = 0;
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);
	fprintf (stdout, "Accion -> Torreta DOWN\n");

	while (torreta->posicion.y>next_move.y) {	
		y_next = torreta->posicion.y - torreta->servo_y.incremento;
	
		if (y_next >= torreta->servo_y.minimo) {
			torreta->posicion.y = y_next;
			pwmWrite(GPIO19, torreta->posicion.y);
		}
		else if(torreta->posicion.y > torreta->servo_y.minimo) {
			torreta->posicion.y = torreta->servo_y.minimo;
			pwmWrite(GPIO19, torreta->posicion.y);
			break;
		}	
	delay(CLK_TRR);
	}
	flags_juego &= ~FLAG_JOYSTICK_DOWN;	
}

void MueveTorretaIzquierda (fsm_t* this) {
	int x_next=0;
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);
	fprintf (stdout, "Accion -> Torreta IZQ\n");
	
	while (torreta->posicion.x<next_move.x) {
		x_next = torreta->posicion.x + torreta->servo_x.incremento;

		if (x_next <= torreta->servo_x.maximo) {
			torreta->posicion.x = x_next;
			pwmWrite(GPIO18, torreta->posicion.x);
		}
		else if(torreta->posicion.x > torreta->servo_x.maximo) {
			torreta->posicion.x = torreta->servo_x.maximo;
			pwmWrite(GPIO18, torreta->posicion.x);
			break;
		}
	delay(CLK_TRR);
	}
	flags_juego &= ~FLAG_JOYSTICK_LEFT;
}

void MueveTorretaDerecha (fsm_t* this) {
	int x_next=0;
	TipoTorreta* torreta = (TipoTorreta*)(this->user_data);
	fprintf (stdout, "Accion -> Torreta DER\n");
	
	while (torreta->posicion.x>next_move.x) {
		x_next = torreta->posicion.x - torreta->servo_x.incremento;

		if (x_next >= torreta->servo_x.minimo) {
			torreta->posicion.x = x_next;
			pwmWrite(GPIO18, torreta->posicion.x);
		}
		else if(torreta->posicion.x < torreta->servo_x.minimo) {
			torreta->posicion.x = torreta->servo_x.minimo;
			pwmWrite(GPIO18, torreta->posicion.x);
			break;
		}
	delay(CLK_TRR);
	}
	flags_juego &= ~FLAG_JOYSTICK_RIGHT;	
}

void DisparoIR (fsm_t* this) {
	flags_juego &= ~FLAG_TRIGGER_BUTTON;
	flags_player |= FLAG_START_DISPARO;
	fprintf (stdout, "Accion -> Disparo\n");
}

void FinalDisparoIR (fsm_t* this) {
	// A completar por el alumno
	// ...	
}

void ImpactoDetectado (fsm_t* this) {
	// A completar por el alumno
	// ...
}

void FinalizaJuego (fsm_t* this) {
	// A completar por el alumno
	// ...
}

void NoHagoNada(fsm_t* this) {
	fprintf (stdout, "No hago nada\n");
}
