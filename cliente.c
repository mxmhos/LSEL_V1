#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include "cliente.h"
//hola

int ComprueboServer(){
  int conexion, aux;
  char buffer[100];
  char cab[10];
  int par1, par2;
  char par3[2], par4[2];

  struct sockaddr_in cliente; 		//Declaración de la estructura con información para la conexión
  struct hostent *servidor; 		//Declaración de la estructura con información del host
  servidor = gethostbyname(host); 	//Asignacion
  
  if(servidor == NULL)
  { //Comprobación
    printf("Host erróneo\n");
    return 1;
  }

  conexion = socket(AF_INET, SOCK_STREAM, 0); //Asignación del socket
  bzero((char *)&cliente, sizeof((char *)&cliente)); //Rellena toda la estructura de 0's
        //La función bzero() es como memset() pero inicializando a 0 todas la variables


  cliente.sin_family = AF_INET; 	//asignacion del protocolo
  cliente.sin_port = htons(puerto); 	//asignacion del puerto
  bcopy((char *)servidor->h_addr, (char *)&cliente.sin_addr.s_addr, sizeof(servidor->h_length));
  //bcopy(); copia los datos del primer elemendo en el segundo con el tamaño máximo 
  //del tercer argumento.


  //cliente.sin_addr = *((struct in_addr *)servidor->h_addr); //<--para empezar prefiero que se usen
  //inet_aton(argv[1],&cliente.sin_addr); //<--alguna de estas dos funciones


  if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0)
  { //conectando con el host
    printf("Error conectando con el host\n");
    close(conexion);
    return 1;
  }
  printf("Conectado con %s:%d\n",inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));
  //inet_ntoa(); está definida en <arpa/inet.h>


//*******************************************

  send(conexion, "cliente", 100, 0); //envio

  bzero(buffer, 100);

  recv(conexion, buffer, 100, 0); //recepción

	char *ptr = strtok(buffer, " ");
	printf("cabecera: '%s'\n", ptr); strcpy(cab, ptr);
	ptr = strtok(NULL, " ");  
	par1=atoi(ptr); //printf("par 1: '%d'\n", par1);
	ptr = strtok(NULL, " ");
	par2=atoi(ptr); //printf("par 2: '%d'\n", par2);
	ptr = strtok(NULL, " ");
	strcpy(par3, ptr); //printf("par 3: '%s'\n", par3);
	ptr = strtok(NULL, " ");
	strcpy(par4, ptr); //printf("par 4: '%s'\n", par4);


	if (strcmp(cab, "clienteC")==0)	{
		//printf("Recibo un dato valido\n");

		aux=(P_MAX + P_MIN)/2 + par1;
		if ((aux<next_move.x-1)||(aux>next_move.x+1)) {
			if (aux>next_move.x)
				flags_juego |= FLAG_JOYSTICK_LEFT;
			if (aux<next_move.x)
				flags_juego |= FLAG_JOYSTICK_RIGHT;
			next_move.x=aux;
			//flags_juego |= FLAG_MOVE; 
		}

		aux=(P_MAX + P_MIN)/2 + par2;
		if ((aux<next_move.y-1)||(aux>next_move.y+1)) {
			if (aux>next_move.y) 
				flags_juego |= FLAG_JOYSTICK_UP;
			if (aux<next_move.y)
				flags_juego |= FLAG_JOYSTICK_DOWN;
			next_move.y=aux;
			//flags_juego |= FLAG_MOVE; 
		}

		if (strcmp(par3, "F")==0)
			flags_juego |= FLAG_TRIGGER_BUTTON;

		if (strcmp(par4, "S")==0)
			flags_juego |= FLAG_TARGET_DONE;

		printf("Pase los flags, con par1 = %d, par3 = %d, par3 = %s, par4 = %s\n", par1, par2, par3, par4);
	}
	else	{
		printf("Recibi algo, pero no era para mi\n");
		par1=0; par2=0; strcmp(par3, "W"); strcmp(par4, "N");
	}

return 0;
}

