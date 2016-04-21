/*
 * 	Modelo ejemplo de un Cliente que envia mensajes a un Server.
 *
 * 	Cada mensaje estara compuesto por 2 numeros que seran sumados en el servidor.
 *
 * 	No se contemplan el manejo de errores en el sistema por una cuestion didactica. Tener en cuenta esto al desarrollar.
 */

#include "Cliente.h"


int main(){
	/*
	 *  ¿Quien soy? ¿Donde estoy? ¿Existo?
	 *
	 *  Estas y otras preguntas existenciales son resueltas getaddrinfo();
	 *
	 *  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	 *
	 */
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion


	/*
	 * 	Ya se quien y a donde me tengo que conectar... ¿Y ahora?
	 *	Tengo que encontrar una forma por la que conectarme al server... Ya se! Un socket!
	 *
	 * 	Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.
	 *
	 */
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	/*
	 * 	Perfecto, ya tengo el medio para conectarme (el archivo), y ya se lo pedi al sistema.
	 * 	Ahora me conecto!
	 *
	 */
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	/*
	 *	Estoy conectado! Ya solo me queda una cosa:
	 *
	 *	Enviar datos!
	 *
	 *	Vamos a crear un paquete con 2 numeros que seran enviados al servidor.
	 *
	 *	Aprovechando el standard imput/output, guardamos los valores que ingresa el usuario en la consola, los mandamos a serializar,
	 *	generando el paquete, y, por ultimo, enviamos.
	 *
	 *	Si ambos operandos son 0, salimos del ciclo.
	 *
	 */
	int enviar = 1;
	t_Package operandos;
	/* Necesitamos asegurarnos de enviar un stream de caracteres contiguo. Por ello, reservamos el espacio. */
	int packageSize = sizeof(operandos.Operando1) + sizeof(operandos.Operando2);
	char *message = malloc(packageSize);


	while(enviar){
		printf("Conectado al servidor. Ingrese dos numeros, para salir, ambos operandos deben ser 0:\n");
		scanf("%d", &(operandos.Operando1));
		scanf("%d", &(operandos.Operando2));
		if((operandos.Operando1 == 0) && (operandos.Operando2 == 0)) enviar = 0;		// Chequea si el usuario quiere salir.
		if(enviar) {
			serializarOperandos(operandos, message);	// Ver: ¿Por que serializar? En el comentario de la definicion de la funcion.
			send(serverSocket, message, packageSize, 0);
		}
	}

	printf("Desconectado\n");
	/*	NUNCA nos olvidamos de liberar la memoria que pedimos.
	 *
	 *  Acordate que por cada free() que no hacemos, valgrind mata a un gatito.
	 */
	free(message);


	/*
	 *	Listo! Cree un medio de comunicacion con el servidor, me conecte con y le envie cosas...
	 *
	 *	...Pero me aburri. Era de esperarse, ¿No?
	 *
	 *	Asique ahora solo me queda cerrar la conexion con un close();
	 */

	close(serverSocket);

	/* ADIO'! */
	return 0;

}


/*
 * 	¿Por que serializar?
 *
 * 	Como ya sabemos, para enviar datos por sockets, debemos enviar un stream de datos. Esto significa que, independientemente de la
 * 	estructura que estemos intentando enviar, los datos deben enviarse de forma contigua y de una forma tal que el destinatario pueda
 * 	identificar inequivocamente lo que le enviemos. Para ello, necesitamos utilizar un formato que hayamos acordado previamente con el receptor.
 * 	Ademas, nosotros tenemos que tener en cuenta que nuestro programa en tiempo de ejecucion puede ubicar las distintas partes de
 * 	una estructura de forma distribuida en la memoria disponible. Esto, basicmente, significa que LOS DATOS PUEDEN NO ESTAR CONTIGUOS.
 *
 * 	Para solucionar esto, los datos se serializan. Serializar significa ubicar los datos a enviar de una forma inequivoca y contigua en un
 * 	cierto espacio de memoria de tal forma que el receptor pueda reconstruir la estructura en su propio espacio de memoria.
 */
void serializarOperandos(t_Package operandos, char* message){

	int offset=0;

	memcpy(message, &(operandos.Operando1), sizeof(operandos.Operando1));

	offset = sizeof(operandos.Operando1); 		// Ya ubicamos la primera parte, ahora debemos corrernos para no sobreescribir.

	memcpy(message + offset, &(operandos.Operando2), sizeof(operandos.Operando2));
}
