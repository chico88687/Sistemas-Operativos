#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 500

typedef struct {
	int tipo; // Tipo de Consulta: 1-Normal, 2-COVID19, 3-Urgente
	char descricao[100]; // Descrição da Consulta
	int pid_consulta; // PID do processo que quer fazer a consulta
} Consulta;

#endif
