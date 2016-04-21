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

/*
 * 	Definicion de estructuras
 *
 * 	Es importante destacar que utilizamos el tipo uint_32, incluida en el header <stdint.h> para mantener un estandar en la cantidad
 * 	de bytes del paquete.
 */
typedef struct _t_Package {
	uint32_t Operando1;
	uint32_t Operando2;
} t_Package;

/*
 * 	Definicion de funciones
 */

void deserializarOperandos(t_Package* , char*);

#endif /* SERVER_H_ */
