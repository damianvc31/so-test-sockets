#Makefile para el modelo Cliente-Servidor presentado como ejemplo.

COMPILER=gcc
BIN_PATH=bin

all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente
	$(COMPILER) Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server

clean:
	rm -rf bin

cliente:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente

server:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server


# Contemplamos algunos Alias para subsanar posibles diferencias de escritura

client:
	make cliente

servidor:
	make server


# Sistemas Operativos, Universidad Tecnologica Nacional, Facultad Regional Buenos Aires. 2014.
