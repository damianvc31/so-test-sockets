/*
 * 	Modelo ejemplo de un Cliente que envia mensajes a un Server.
 *
 * 	Al iniciar, el cliente ingresara su nombre de usuario. Luego, podra enviar mensajes al servidor.
 *
 * 	No se contemplan el manejo de errores en el sistema por una cuestion didactica. Tener en cuenta esto al desarrollar.
 */

#include "Cliente.h"


int main(){
	/*
	 * 	Obtendo el username, que despues utilizare para "identificarme" con el server (lo envio en el paquete).
	 */
	char *username = malloc(MAXUSERNAME);
	get_Username(username);

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

	printf("Conectado al servidor.\n");
	/*
	 *	Estoy conectado! Ya solo me queda una cosa:
	 *
	 *	Enviar datos!
	 *
	 *	Debemos obtener el mensaje que el usuario quiere mandar al chat, y luego serializarlo.
	 *
	 *	Si el mensaje es "exit", saldremos del sistema.
	 *
	 */
	int enviar = 1;
	t_Package package;
	package.message = malloc(MAX_MESSAGE_SIZE);
	char *serializedPackage;


	printf("Bienvenido al sistema, puede comenzar a escribir. Escriba 'exit' para salir.\n");

	while(enviar){

		fill_package(&package, username);						// Completamos el package, que contendra los datos del usuario y los datos del mensaje que vamos a enviar.

		if(!strcmp(package.message, "exit\n")) enviar = 0;		// Chequeamos si el usuario quiere salir.

		if(enviar) {
			serializedPackage = serializarOperandos(&package);	// Ver: ¿Por que serializacion dinamica? En el comentario de la definicion de la funcion.
			send(serverSocket, serializedPackage, package.total_size, 0);
			dispose_package(serializedPackage);
		}
	}

	printf("Desconectado.\n");

	/*	NUNCA nos olvidamos de liberar la memoria que pedimos.
	 *
	 *  Acordate que por cada free() que no hacemos, valgrind mata a un gatito.
	 */
	free(package.message);
	free(username);

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
 * 	¿Por que serializacion dinamica?
 * 	(Leer previamente ¿Por que serializacion? en el branch StaticSerialization.
 *
 * 	Como vimos antes, le tenemos que mandar un mensaje al receptor que contiene algunos valores. El receptor, por su parte, lo analizara y dividira en partes
 * 	para luego poder operar con ellos. Pero... ¿Que pasa si la cantidad de datos (o su largo) es variable?
 * 	Si le mandamos los datos sin importar, el tipo no sabria decir cuantos datos buscar o que tan grandes son los datos que le pasamos. Por ende, no los podria
 * 	identificar, entoooonces... PUM! Rompe.
 *
 * 	Para solucionar esto, surge el concepto de serializacion dinamica (se lo imaginaban, ¿no?). Esto consiste en encontrar una forma de comunicarle al receptor
 * 	cuantos o de que size sera cada uno de los valores que le pasamos.
 * 	De la misma forma que no hay solo una tecnica para serializar estaticamente, tampoco hay una sola forma de serializar dinamicamente. Cada problema puede
 * 	encontrar distintas soluciones, y no existe una forma universal de delimitar datos. Pero, en funcion de nuestro problema (enviar un nombre de usuario y
 * 	un mensaje), veamos algunas:
 *
 * 	La solucion utilizada consiste en obtener el length de tanto el nombre de usuario como el mensaje. Unimos estos dos valores al paquete inicial (asegurandonos
 * 	de enviar primero el largo de cada string y luego el string) y de esta forma el receptor podra saber cuantos caracteres debe esperar (o leer) y asi poder
 * 	generar datos con los que pueda operar.
 * 	Otra solucion bastante inteligente, que se encuentra implementada en el Parser del TP del 1C-2014: EstaCover Flow, es la separacion de las cadenas de string
 * 	mediante un caracter centinela. Cualquier estudiante aplicado de SSL sabra que el \0 es el caracter centinela por excelencia, y eso no escapa al Parser.
 *
 * 	Entonces... Veamos un poco mas de codigo:
 *
 */
char* serializarOperandos(t_Package *package){

	char *serializedPackage = malloc(package->total_size);

	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(package->username_long);
	memcpy(serializedPackage + offset, &(package->username_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->username_long;
	memcpy(serializedPackage + offset, package->username, size_to_send);
	offset += size_to_send;

	size_to_send =  sizeof(package->message_long);
	memcpy(serializedPackage + offset, &(package->message_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->message_long;
	memcpy(serializedPackage + offset, package->message, size_to_send);

	return serializedPackage;
}


/*
 * 	Funciones auxiliares
 */

void get_Username(char *username){

	printf("Ingrese su nombre de usuario: ");

	fgets(username, MAXUSERNAME, stdin);

	/* Como fgets incluye el \n al final del username, nosotros se lo sacamos: */
	int username_long = strlen(username);
	username[username_long-1] = '\0';


}

void fill_package(t_Package *package, char* username){
	/* Me guardo los datos del usuario y el mensaje que manda */
	package->username = username;
	package->username_long = strlen(username) + 1; 		// Me guardo lugar para el \0

	fgets(package->message, MAX_MESSAGE_SIZE, stdin);
	//(package->message)[strlen(package->message)] = '\0'; esa linea no hace nada

	package->message_long = strlen(package->message) + 1;	// Me guardo lugar para el \0

	package->total_size = sizeof(package->username_long) + package->username_long + sizeof(package->message_long) + package->message_long;
	// Si, este ultimo valor es calculable. Pero a fines didacticos la calculo aca y la guardo a futuro, ya que no se modificara en otro lado.
}

void dispose_package(char *package){
	free(package);
}
