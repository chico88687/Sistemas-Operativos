#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/msg.h>


#define print_on_error(s,m) if (s<0) {perror(m); /* exit(1); */}
#define print_on_null(s,m) if (s==NULL) { perror(m); /* exit(1); */ }

#define IPC_KEY 0x0a88687
#define IPC_KEYB 0x0b88687

#define SIZE 500
#define NCONSULTAS 10

struct sembuf UP = {0, 1, 0};
struct sembuf DOWN = {0, -1, 0};

typedef struct {
	int tipo; // Tipo de Consulta: 1-Normal, 2-COVID19, 3-Urgente
	char descricao[100]; // Descrição da Consulta
	int pid_consulta; // PID do processo que quer fazer a consulta
	int status; // Estado da consulta: 1-Pedido, 2-Iniciada,
} Consulta; // 3-Terminada, 4-Recusada, 5-Cancelada

typedef struct {
	long tipo;
	Consulta consulta;	
} Mensagem;

typedef struct {
	int cont_1;
	int cont_2;
	int cont_3;
	int cont_perdida;	
}Contador;

void fix_fgets(char* string,int size){
    string[size - 1] = 0;
}

#endif
