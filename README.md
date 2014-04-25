Ejemplos de comunicación por Sockets.
==============

Este repositorio se encuentra compuesto por tres branches que contienen ejemplos (incrementales en dificultad) sobre el uso de sockets.


Organización
--------------

Se puede encontrar un ejemplo en cada branch, siguiendo lo siguiente:

* Master: Ejemplo básico de un Cliente que envia mensajes de texto a un servidor que lo muestra por pantalla.
* StaticSerialization:  Ejemplo de serialización estática, en la que el Cliente envía dos números al Servidor y este muestra el resultado de la suma de ambos en pantalla. 
* DinamicSerialization: Ejemplo de serialización dinámica, en la que el Cliente settea un username y le envía mensajes con dicha autenticación al Servidor y este lo muestra por pantalla.


¿Cómo obtengo el repositorio?
--------------

Utilizando la <ruta> del repositorio:

    git clone <ruta>


¿Cómo me muevo entre los ejemplos?
--------------

En la ruta del repositorio ya clonado:

    git checkout <nombreBranch>

En donde <nombreBranch> es alguno de los mencionados en la sección de organización.


¿Cómo pruebo los ejemplos sin pasarlos al Eclipse?
--------------

En la ruta del repositorio, ya situados en el branch correspondiente al ejemplo a testear:

    make all                    // Crea una carpeta bin/ con ambos ejecutables
    
Utilizar una consola diferente para abrir cada ejecutable con:

    ./bin/Server
    ./bin/Cliente
    

¿Y si los quiero importar al Eclipse?
--------------

Mirate este video del gran MGI: https://www.youtube.com/watch?v=772fLAsut3E

