/*
 * Cliente.h
 *
 *  Created on: 23/04/2014
 *      Author: maximilianofelice
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

/*
 * 	Definicion de estructuras
 *
 * 	Es importante destacar que utilizamos el tipo uint_32, incluida en el header <stdint.h> para mantener un estandar en la cantidad
 * 	de bytes del paquete.
 *
 */
typedef struct _t_Package {
	char* username;
	uint32_t username_long;
	char* message;
	uint32_t message_long;
	uint32_t total_size;			// NOTA: Es calculable. Aca lo tenemos por fines didacticos!
} t_Package;

/*
 * 	Definicion de funciones
 */

char* serializarOperandos(t_Package*);
void get_Username(char**);
void fill_package(t_Package*, char**);
void dispose_package(char**);

#endif /* CLIENTE_H_ */
