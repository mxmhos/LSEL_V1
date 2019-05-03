
#include "piTankGo_1.h"

int frecuenciaStarwars[59] = {523,0,523,0,523,0,698,0,1046,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,1760,0,0,784,0,523,0,0,523,0,0,523,0};

int tiempoStarwars[59] = {134,134,134,134,134,134,536,134,536,134,134,134,134,134,134,536,134,402,134,134,429,357,134,134,134,134,536,134,402,134,134,429,357,134,134,134,134,536,134,402,134,134,429,357,134,134,134,429,357,1071,268,67,67,268,67,67,67,67,67};

int frecuenciasDisparo[16] = {2500,2400,2300,2200,2100,2000,1900,1800,1700,1600,1500,1400,1300,1200,1100,1000};

int tiemposDisparo[16] = {75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75};

int frecuenciasImpacto[32] = {97,109,79,121,80,127,123,75,119,96,71,101,98,113,92,70,114,75,86,103,126,118,128,77,114,119,72};

int tiemposImpacto[32] = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

int flags_player = 0;
int flags_system = 0;
int estado;
int modo;
TipoPosicionTorreta next_move;

tmr_t* timer_efecto;

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraSistema (TipoPlayer *p_player) {
	int result = 0;

	//Inicialización de la configuración de wiringPi-completamente necesario
	wiringPiSetupGpio();
	wiringPiSetup(); // y esta?
    
	//creación del hilo para el control del sonido
	softToneCreate (PLAYER_PWM_PIN);
	softToneWrite(PLAYER_PWM_PIN, NO_SONAR);

	//Configuro efectos de sonido en Player
	InicializaEfecto (&(p_player->efecto_disparo), "Disparo", frecuenciasDisparo, tiemposDisparo, 16);
	InicializaEfecto (&(p_player->efecto_impacto), "Impacto", frecuenciasImpacto, tiemposImpacto, 32);
	InicializaEfecto (&(p_player->efecto_fin), "Fin", frecuenciaStarwars, tiempoStarwars, 59);

	//Funcion que se va a llamar cuando se produzca la interrupción
	timer_efecto = tmr_new (timer_player_duracion_nota_actual_isr);

	//Configuro Pin de Laser
	pinMode (LASER_PIN, OUTPUT);

	return result;
}

//procedimiento de inicializacion del sistema.
int InicializaSistema () {
	int result = 0;

	//Inicializo variables gobales
	estado=1;
	modo=1;

	//Inicializo varaible next_move
	next_move.x= (P_MAX+P_MIN)/2;
	next_move.y= P_MIN;

	//Inicializo Pin de Laser
	digitalWrite (LASER_PIN,  LOW); 

	// Lanzamos thread para movimiento de servos
	result = piThreadCreate (thread_torreta);
	if (result!=0) {
		printf ("Thread de servos didn't start!!!\n");
	return -1;
    	}

	return result;
}

//------------------------------------------------------
// THREADS DE MOVIMIENTO
//------------------------------------------------------
PI_THREAD (thread_torreta) {
	TipoTorreta torreta;
	InicializaTorreta (&torreta);
	while(1) {
		delay(CLK_TRR);
		if (estado==1)
			ComprueboTorreta(&torreta);
	}
}


//------------------------------------------------------
// wait until next_activation (absolute time)
//------------------------------------------------------
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
	}
}


//------------------------------------------------------
// main: fsm de sonido. Comms y torreta thread aparte.
//------------------------------------------------------
int main ()
{
	TipoPlayer player;
	unsigned int next;

	// Configuracion e inicializacion del sistema
	ConfiguraSistema (&player);
	InicializaSistema ();
	mqtt_init();

	fsm_trans_t reproductor[] = {
		{ WAIT_START, CompruebaStartDisparo, WAIT_NEXT, InicializaPlayDisparo },
		{ WAIT_START, CompruebaStartImpacto, WAIT_NEXT, InicializaPlayImpacto },
		{ WAIT_START, CompruebaStartEnd, GAME_END, InicializaEndGame },
		{ WAIT_NEXT, CompruebaStartImpacto, WAIT_NEXT, InicializaPlayImpacto },
		{ WAIT_NEXT, CompruebaNotaTimeout, WAIT_END, ActualizaPlayer },
		{ WAIT_END, CompruebaFinalEfecto, WAIT_START, FinalEfecto },
		{ WAIT_END, CompruebaNuevaNota, WAIT_NEXT, ComienzaNuevaNota},
		{-1, NULL, -1, NULL },
	};

	fsm_t* player_fsm = fsm_new (WAIT_START, reproductor, &(player));

	//Hilo de sonido (main)
	next = millis();
	while (1) {
		if (estado==1)
			fsm_fire (player_fsm);
		if (modo==0)
			digitalWrite (LASER_PIN, HIGH);
		next += CLK_PLY;
		delay_until (next);
	}
	return 0;
}
