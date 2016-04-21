/*
 * Modelo ejemplo de un servidor que espera mensajes de un proceso Cliente que se conecta a un cierto puerto.
 * Al recibir un mensaje, lo imprimira por pantalla.
 */

#include "Server.h"

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
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE


	/*
	 * 	Descubiertos los misterios de la vida (por lo menos, para la conexion de red actual), necesito enterarme de alguna forma
	 * 	cuales son las conexiones que quieren establecer conmigo.
	 *
	 * 	Para ello, y basandome en el postulado de que en Linux TODO es un archivo, voy a utilizar... Si, un archivo!
	 *
	 * 	Mediante socket(), obtengo el File Descriptor que me proporciona el sistema (un integer identificador).
	 *
	 */
	/* Necesitamos un socket que escuche las conecciones entrantes */
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	/*
	 * 	Perfecto, ya tengo un archivo que puedo utilizar para analizar las conexiones entrantes. Pero... ¿Por donde?
	 *
	 * 	Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	 *
	 * 				OJO! Todavia no estoy escuchando las conexiones entrantes!
	 *
	 */
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	/*
	 * 	Ya tengo un medio de comunicacion (el socket) y le dije por que "telefono" tiene que esperar las llamadas.
	 *
	 * 	Solo me queda decirle que vaya y escuche!
	 *
	 */
	listen(listenningSocket, BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.

	/*
	 * 	El sistema esperara hasta que reciba una conexion entrante...
	 * 	...
	 * 	...
	 * 	BING!!! Nos estan llamando! ¿Y ahora?
	 *
	 *	Aceptamos la conexion entrante, y creamos un nuevo socket mediante el cual nos podamos comunicar (que no es mas que un archivo).
	 *
	 *	¿Por que crear un nuevo socket? Porque el anterior lo necesitamos para escuchar las conexiones entrantes. De la misma forma que
	 *	uno no puede estar hablando por telefono a la vez que esta esperando que lo llamen, un socket no se puede encargar de escuchar
	 *	las conexiones entrantes y ademas comunicarse con un cliente.
	 *
	 *			Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
	 *				En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.
	 *
	 */
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	/*
	 * 	Ya estamos listos para recibir paquetes de nuestro cliente...
	 *
	 * 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y luego imprimiremos el mensaje por pantalla.
	 *
	 *	Cuando el cliente cierra la conexion, recieve_and_deserialize() devolvera 0.
	 */
	t_Package package;
	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando Envío de mensajes.\n");

	while (status){
		status = recieve_and_deserialize(&package, socketCliente);					// Ver el "Deserializando estructuras dinamicas" en el comentario de la funcion.
		if (status) printf("%s says: %s", package.username, package.message);
	}


	printf("Cliente Desconectado.\n");
	/*
	 * 	Terminado el intercambio de paquetes, cerramos todas las conexiones y nos vamos a mirar Game of Thrones, que seguro nos vamos a divertir mas...
	 *
	 *
	 * 																					~ Divertido es Disney ~
	 *
	 */
	close(socketCliente);
	close(listenningSocket);

	/* See ya! */

	return 0;
}


/*
 * 	Deserializando estructuras dinamicas
 * 	(leer previamente ¿Por que serializacion dinamica? en el comentario de la serializacion)
 *
 * 	De la misma forma que no existe una sola forma de serializar las estructuras, tampoco de serializarlas. En este caso, realizaremos una deserializacion
 * 	a medida que vamos recibiendo el paquete.
 * 	Para esto es necesario conocer lo siguiente: Si el emisor nos envia un paquete con "ABCDE" y uno lee 2 caracteres, en el buffer interno del sistema quedara
 * 	"CDE" para leer. Por lo tanto, la proxima vez que leamos un caracter, obtendremos "C".
 *
 * 	Aprovechamos eso para modelar la siguiente solucion: Recibiremos la cantidad de caracteres a leer previo a los caracteres, por lo tanto, sabremos cuanto es
 * 	el largo de cada uno de los strings que nos enviaron.
 * 	¿Por que no nos enviaron un MAX_SIZE? Porque nos conectamos en red, y no esta bueno leer, por ejemplo, 300 caracteres, para solamente utilizar 15.
 *
 */
int recieve_and_deserialize(t_Package *package, int socketCliente){

	int status;
	int buffer_size;
	char *buffer = malloc(buffer_size = sizeof(uint32_t));

	status = recv(socketCliente, buffer, sizeof(package->username_long), 0);
	if (!status) return 0;
	memcpy(&(package->username_long), buffer, buffer_size);

	status = recv(socketCliente, package->username, package->username_long, 0);
	if (!status) return 0;


	status = recv(socketCliente, buffer, sizeof(package->message_long), 0);
	if (!status) return 0;
	memcpy(&(package->message_long), buffer, buffer_size);

	status = recv(socketCliente, package->message, package->message_long, 0);
	if (!status) return 0;


	free(buffer);

	return status;
}

