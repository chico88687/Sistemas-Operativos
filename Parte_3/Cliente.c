 #include "includes.h"

Consulta consulta;
int msg_id;

void trata_SIGINT(int n){
	printf("\nPaciente cancelou o pedido\n");
	Mensagem m;
    m.tipo = getpid();
    m.consulta = consulta;
	m.consulta.status = 5;
	    
	int status = msgsnd(msg_id, &m, sizeof(m.consulta), 0);
    print_on_error(status, "Erro em mandar a Mensagem");

	exit(0);
}

void liga_Mensagem(){
    msg_id = msgget( IPC_KEY, 0 );
    print_on_error(msg_id, "A fila de mensagem não existe");
}

void manda_mensagem(){
    Mensagem m;
    m.tipo = 1;
    m.consulta = consulta;
    int status = msgsnd(msg_id, &m, sizeof(m.consulta), 0);
    print_on_error(status, "Erro em mandar a Mensagem");
//    printf("Cliente Pid = %d\n", consulta.pid_consulta);
}

int tipo_aceitar(char* s_tipo, int size){
	if(size != 1)
		return 0;
	else{
		if('1' <= s_tipo[0] && s_tipo[0] <= '3')
			return 1;
		else
			return 0;
	}	
}

void cria_Consulta(){
	char s_tipo[SIZE];
	int aux = 0;
	int t = 0;
	
	while(!tipo_aceitar(s_tipo, t)){
		if(aux == 0)
			aux++;
		else
			printf("Dados inválidos\n");
		printf("Tipo de Consulta: 1-Normal, 2-COVID19, 3-Urgente\n");
		printf("%s", "Insira o tipo de consulta: ");
		fgets(s_tipo, SIZE, stdin);
		fix_fgets(s_tipo, strlen(s_tipo));
		t =strlen(s_tipo);
	}
	char descricao[SIZE];	
	printf("%s","Insira a descrição da consulta: " );
	fgets(descricao,SIZE,stdin);
	int size = strlen(descricao);
	fix_fgets(descricao, size);;
	printf("Tipo = %s, descrição = %s\n", s_tipo, descricao);

	consulta.tipo = atoi(s_tipo);
	strcpy(consulta.descricao, descricao);
	consulta.pid_consulta = getpid();
	consulta.status = 1;
	manda_mensagem();
}

int esperar_resposta(){
	Mensagem m;
	printf("Aguarde\n");
	int status = msgrcv(msg_id, &m, sizeof(m.consulta), getpid() , 0);
	print_on_error(status,"Erro ao receber resposta.")
	
	if(m.consulta.status == 4){
		printf("Consulta não é possível para o processo <%d>\n", getpid());
		return 1;
	}

	if(m.consulta.status == 2){
		printf("Consulta iniciada para o processo <%d>\n", getpid());
		return 0;
	}

	if(m.consulta.status == 3){
		printf("Consulta concluída para o processo <%d>\n", getpid());
		return 1;
	}
	return 0;
}


int main(){	
	printf("Consulta id = %d\n", getpid());
	signal(SIGINT, trata_SIGINT);

	liga_Mensagem();
	cria_Consulta();
	int termina = 0;
	while(!termina)
		termina = esperar_resposta();
}
