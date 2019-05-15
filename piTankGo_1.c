
#include "piTankGo_1.h"

int efecto;
int estado;
int modo;
int boton;
TipoPosicionTorreta next_move;

tmr_t* timer_laser;

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraSistema () {
	int result = 0;

	//Inicialización de la configuración de wiringPi-completamente necesario
	wiringPiSetupGpio();
	wiringPiSetup(); // y esta?
    
	//Funcion que se va a llamar cuando se produzca la interrupción
	timer_laser = tmr_new (timer_laser_duracion_disparo_isr);

	//Configuro Pin de Laser
	pinMode (LASER_PIN, OUTPUT);

	return result;
}

//procedimiento de inicializacion del sistema.
int InicializaSistema () {
	int result = 0;

	//Inicializo variables gobales
	estado=1;
	modo=0;
	efecto=0;

	//Inicializo varaible next_move
	next_move.x= (P_MAX+P_MIN)/2;
	next_move.y= P_MIN;

	//Inicializo Pin de Laser
	digitalWrite (LASER_PIN,  LOW); 

	return result;
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
// main: ecfectos de sonido. Comms thread aparte.
//------------------------------------------------------
int main ()
{
	unsigned int next;
	TipoTorreta torreta;
	
	// Configuracion e inicializacion del sistema
	ConfiguraSistema ();
	InicializaSistema ();
	InicializaTorreta (&torreta);
	mqtt_init();

	//Programa principal
	next = millis();
	while (1) {
		if (estado==1) {
			ComprueboTorreta(&torreta);
			if ((efecto==1) && (modo==1)) {		//DISPARO 
				//system("mplayer Laser_Gun.mp3");
				printf("\nmplayer Laser + HIGH_pin\n");
				digitalWrite (LASER_PIN, HIGH);
				tmr_startms(timer_laser, DURACION_LASER);
				efecto=0;
			}
			if (efecto==2) { 			//ATINO
				//system("mplayer Esplosion.mp3");
				printf("\nmplayer Explosion\n");
				efecto=0;
			}	
			if (modo==0)
				digitalWrite (LASER_PIN, HIGH);
		}
		else {
			digitalWrite (LASER_PIN, LOW);
		}
		next += CLK_MAIN;
		delay_until (next);
	}
	return 0;
}


//------------------------------------------------------
// PROCEDIMIENTOS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
void timer_laser_duracion_disparo_isr (union sigval value) {
	if (modo==1) {
		digitalWrite (LASER_PIN, LOW);
		printf("LOWEReo el PIN\n");
	}
}

