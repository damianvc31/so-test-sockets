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
	 * 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y luego imprimiremos la suma de los operandos por pantalla.
	 *
	 *	Cuando el cliente cierra la conexion, recv() devolvera 0.
	 */
	t_Package operandos;
	int packageSize = sizeof(operandos.Operando1) + sizeof(operandos.Operando2);
	char *package = malloc(packageSize);
	uint32_t suma;

	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente Conectado. ");

	while (status != 0){
		printf("Esperando valores...\n");
		status = recv(socketCliente, (void*) package, packageSize, 0);
		deserializarOperandos(&(operandos), &(package));		// Ver: ¿Por que deserializar? En el comentario de la definicion de la funcion.
		suma = operandos.Operando1 + operandos.Operando2;
		if (status != 0) printf("La suma es %d\n", suma);

	}

	printf("Cliente Desconectado.\n");

	/*	NUNCA nos olvidamos de liberar la memoria que pedimos.
	 *
	 *  Acordate que por cada free() que no hacemos, valgrind mata a un gatito.
	 */
	free(package);

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
 * 	¿Por que deserializar?
 * 	(leer primero: ¿Por que serializar? en el proceso cliente)
 *
 * 	De forma inversa a la serializacion, los datos deben ser reconstruidos en el receptor, para que este pueda trabajar con ellos.
 * 	Cabe destacar que no necesariamente el formato de los datos enviados es el mismo en el emisor que en el receptor. Por ejemplo,
 * 	el caso mas representativo es la diferencia en el tipo de almacenamiento de datos en memoria que estos usen. Si uno utiliza
 * 	Big Endian, y otro Little Endian, al realizar el envio de datos muy probablemente encontremos una inconsistencia. El desarrollador
 * 	debe tener en cuenta todos estos atenuantes y pensar su solucion de tal forma que sea lo mas portable posible (al fin y al cabo,
 * 	para eso queremos comunicarnos por red, ¿no?). Investigar tambien, el caso de la serializacion en C de datos con punto flotante.
 *
 * 	Por suerte, mucha gente antes que nosotros se encontro con este problema, y desarrollaron una serie de estandares para que todos
 * 	podamos desarrollar de una forma feliz y eficiente. Funciones como htons(), nos permiten convertir ciertos tipos de datos a un estandar.
 *
 * 	Recapitulando, deserializar significa tomar el stream de datos recibidos y obtener los datos que alli se encuentran, de tal forma
 * 	que podamos operar con ellos.
 */
void deserializarOperandos(t_Package *operandos, char **package){

	int offset = 0;

	memcpy(&operandos->Operando1, *package, sizeof(operandos->Operando1));

	offset = sizeof(operandos->Operando1);

	memcpy(&operandos->Operando2, *package + offset, sizeof(operandos->Operando2));
}
