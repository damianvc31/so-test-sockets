/*
 * Server.h
 *
 *  Created on: 23/04/2014
 *      Author: maximilianofelice
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>


#define PUERTO "6667"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

/*
 * 	Definicion de estructuras
 *
 * 	Es importante destacar que utilizamos el tipo uint_32, incluida en el header <stdint.h> para mantener un estandar en la cantidad
 * 	de bytes del paquete.
 */
typedef struct _t_Package {
	char username[MAXUSERNAME];
	uint32_t username_long;
	char message[MAX_MESSAGE_SIZE];
	uint32_t message_long;
} t_Package;

/*
 * 	Definicion de funciones
 */

int recieve_and_deserialize(t_Package *,int);

#endif /* SERVER_H_ */
