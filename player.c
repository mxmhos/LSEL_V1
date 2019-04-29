
#include "player.h"
#include "softTone.h"
#include "tmr.h"
#include "string.h"


extern tmr_t* timer_efecto;

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------
void InicializaEfecto (TipoEfecto *p_efecto, char *nombre, int *array_frecuencias, int *array_duraciones, int num_notas)
{
	strcpy(p_efecto->nombre, nombre);
	for(int i = 0; i < num_notas; i++) {
		p_efecto->frecuencias[i] = array_frecuencias[i];
		p_efecto->duraciones[i] = array_duraciones[i];
	}
	p_efecto->num_notas = num_notas;
}

void InicializaPlayer (TipoPlayer *p_player)
{
	p_player->posicion_nota_actual = 0;
	p_player->frecuencia_nota_actual = p_player->p_efecto->frecuencias[0];
	p_player->duracion_nota_actual = p_player->p_efecto->duraciones[0];
	softToneWrite (PLAYER_PWM_PIN, p_player->frecuencia_nota_actual);//primera nota a sonar
	//se le pasa el timer asociado y se le pasa el tiempo en milisegundos que dura la nota
	tmr_startms(timer_efecto, p_player->duracion_nota_actual);
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
int CompruebaStartDisparo (fsm_t* this)
{
	int result = 0;
	if (flags_player & FLAG_START_DISPARO) {
		result = 1;
	}
	return result;
}

int CompruebaStartImpacto (fsm_t* this)
{
	int result = 0;
	if (flags_player & FLAG_START_IMPACTO) {
		result = 1;
	}
	return result;
}

int CompruebaStartEnd (fsm_t* this)
{
	int result = 0;
	if (flags_player & FLAG_GAME_END) {
		result = 1;
	}
	return result;
}

int CompruebaNuevaNota (fsm_t* this)
{
	int result = 0;
	if (!(flags_player & FLAG_PLAYER_END)) {
		result = 1;
	}
	return result;
}

int CompruebaNotaTimeout (fsm_t* this)
{
	int result = 0;
	if (flags_player & FLAG_NOTA_TIMEOUT) {
		result = 1;
	}
	return result;
}

int CompruebaFinalEfecto (fsm_t* this)
{
	int result = 0;
	if (flags_player & FLAG_PLAYER_END) {
		result = 1;
	}
	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
void InicializaPlayDisparo (fsm_t* this)
{
	TipoPlayer* player = (TipoPlayer*)(this->user_data);
	player->p_efecto = &(player->efecto_disparo);
	fprintf (stdout, "Sonido -> Inicio sonido disparo\n");

	InicializaPlayer (player);//se le pasa toda la estrucctura player
	flags_player &= ~FLAG_START_DISPARO;//se deshabilita el flag de disparo
	digitalWrite (LASER_PIN, HIGH);
}

void InicializaPlayImpacto (fsm_t* this)
{
	TipoPlayer* player = (TipoPlayer*)(this->user_data);
	player->p_efecto = &(player->efecto_impacto);
	fprintf (stdout, "Sonido -> Inicio sonido impacto\n");

	InicializaPlayer (player);
	flags_player &= ~FLAG_START_IMPACTO;//se deshabilita el flag para que no vuelva a entrar por el mismo flag
}

void InicializaEndGame (fsm_t* this)
{
	TipoPlayer* player = (TipoPlayer*)(this->user_data);
	player->p_efecto = &(player->efecto_fin);
	fprintf (stdout, "Sonido -> Inicio sonido fin\n");

	InicializaPlayer (player);
	flags_player &= ~FLAG_GAME_END;//se deshabilita el flag para que no vuelva a entrar por el mismo flag
}

void ComienzaNuevaNota (fsm_t* this)
{
	TipoPlayer* player = (TipoPlayer*)(this->user_data);
	softToneWrite (PLAYER_PWM_PIN, player->frecuencia_nota_actual);
   	tmr_startms(timer_efecto, player->duracion_nota_actual);
	fprintf (stdout, "Sonido -> Comienza nueva nota\n");
}

void ActualizaPlayer (fsm_t* this)
{
	TipoPlayer* player = (TipoPlayer*)(this->user_data);
	int notaActual;

	if (player->posicion_nota_actual + 1 < player->p_efecto->num_notas) {
		notaActual = player->posicion_nota_actual++;
		player->frecuencia_nota_actual = player->p_efecto->frecuencias[notaActual];
		player->duracion_nota_actual = player->p_efecto->duraciones[notaActual];
		//printf("%d\n", player->frecuencia_nota_actual);
	}
	else 
		flags_player |= FLAG_PLAYER_END;

	flags_player &= ~FLAG_NOTA_TIMEOUT;//se baja el flag del time out
}

void FinalEfecto (fsm_t* this)
{
	softToneWrite (PLAYER_PWM_PIN, NO_SONAR);//probar con softStop(int pin)
	digitalWrite (LASER_PIN, LOW);
	flags_player &= ~FLAG_PLAYER_END;//quitamos el flag de final de efecto
}


//------------------------------------------------------
// PROCEDIMIENTOS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
void timer_player_duracion_nota_actual_isr (union sigval value)
{
	//tmr_stop (timer_efecto);//paramos el timer del efecto
	flags_player |= FLAG_NOTA_TIMEOUT;//se activa el flag de tiempo cumplido de la nota   
}
