#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#include "cliente.h"

int ComprueboServer(){
	int conexion, aux;
	char buffer[100];
	char cab[10];
	int par1, par2;
	char par3[2], par4[2];

	struct sockaddr_in cliente; 		//Declaración de la estructura con información para la conexión
	struct hostent *servidor; 		//Declaración de la estructura con información del host
	servidor = gethostbyname(host); 	//Asignacion

	if(servidor == NULL) { //Comprobación
		printf("Host erróneo\n");
		return 1;
	}

	conexion = socket(AF_INET, SOCK_STREAM, 0); //Asignación del socket
	bzero((char *)&cliente, sizeof((char *)&cliente)); //Rellena toda la estructura de 0's

	cliente.sin_family = AF_INET; 	//asignacion del protocolo
	cliente.sin_port = htons(puerto); 	//asignacion del puerto
	bcopy((char *)servidor->h_addr, (char *)&cliente.sin_addr.s_addr, sizeof(servidor->h_length));

	if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0) { //conectando con el host
		printf("Error conectando con el host\n");
		close(conexion);
		return 1;
	}
	//printf("Conectado con %s:%d\n",inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));


//*******************************************
	send(conexion, "cliente", 100, 0); //envio
	bzero(buffer, 100);
	recv(conexion, buffer, 100, 0); //recepción

	char *ptr = strtok(buffer, " ");
	strcpy(cab, ptr); //printf("cabecera: '%s'\n", cab); 
	ptr = strtok(NULL, " ");  
	par1=atoi(ptr); //printf("par 1: '%d'\n", par1);
	ptr = strtok(NULL, " ");
	par2=atoi(ptr); //printf("par 2: '%d'\n", par2);
	ptr = strtok(NULL, " ");
	strcpy(par3, ptr); //printf("par 3: '%s'\n", par3);
	ptr = strtok(NULL, " ");
	strcpy(par4, ptr); //printf("par 4: '%s'\n", par4);

	if (strcmp(cab, "clienteC")==0)	{
		aux=(P_MAX + P_MIN)/2 + par1;
		if ((aux<next_move.x-1)||(aux>next_move.x+1))
			next_move.x=aux;

		aux= P_MAX + par2;
		if ((aux<next_move.y-1)||(aux>next_move.y+1))
			next_move.y=aux;

		if (strcmp(par3, "F")==0)
			flags_player |= FLAG_START_DISPARO;

		if (strcmp(par4, "S")==0) {
			flags_player |= FLAG_START_IMPACTO;
		}

		//printf("Pase los flags, con next_move.x = %d, next_move.y = %d, par3 = %s, par4 = %s\n", next_move.x, next_move.y, par3, par4);
	}

	return 0;
}

