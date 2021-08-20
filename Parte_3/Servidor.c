#include "includes.h"

int msg_id;

int shm_id;
int CountSHM_id;

int semM_id;
int semC_id;

int filhos[NCONSULTAS];

Mensagem dedicada;
int sala_consulta;

int pai;
int cliente;

void init();

/* void trata_SIGCHLD(){
	printf("%d Entra no SIGCHLD\n", getpid());
	wait(NULL);
	printf("Fez o wait\n");
	init();
}

void trata_SIGUSR1(int n){
	printf("[%d] recebeu o SIGUSR1\n",getpid());
	printf("Faz wait\n");
	wait(NULL);
	printf("Volta ao ciclo\n");
	init();
} */

void cria_sem(){
	semM_id = semget(IPC_KEY, 1, IPC_CREAT | 0600 );
    print_on_error(semM_id, "semget");

	int status1 = semctl(semM_id, 0, SETVAL, 1);
    print_on_error(status1, "semctl(SETVAL_19)");

	semC_id = semget(IPC_KEY, 1, IPC_CREAT | 0600 );
    print_on_error(semC_id, "semget");

	int status2 = semctl(semC_id, 0, SETVAL, 1);
    print_on_error(status2, "semctl(SETVAL_26)");
}

void kill_Zombies(int n){
//	printf("Entrou no kill zombies\n");
	wait(NULL);
	init();
}

void trata_SIGINT(int n){
//	printf("O processo %d recebeu o ^C\n", getpid());
	if(pai == getpid()){
		Contador* cont_mem =  (Contador *) shmat(CountSHM_id, 0 , 0);
		print_on_null(cont_mem, "Erro ao escrever em cont_mem")
		
		int sem_status = semop(semC_id, &DOWN, 1);
		print_on_error(sem_status, "DOWN_C(trata_SIGINT)");
		
		printf("\n-------Contador-------\n");
		printf("Número de consultas normais: %d\n", cont_mem[0].cont_1);	
		printf("Número de consultas COVID19: %d\n", cont_mem[0].cont_2);	
		printf("Número de consultas Urgentes: %d\n", cont_mem[0].cont_3);	
		printf("Número de consultas perdidas: %d\n", cont_mem[0].cont_perdida);
		
		sem_status = semop(semC_id, &UP, 1);
		print_on_error(sem_status, "UP_C(trata_SIGINT)");

		exit(0);
	}
} 

void liga_Count(){
	CountSHM_id = shmget( IPC_KEYB, sizeof(Contador) , 0);
/* 	print_on_error(CountSHM_id, "Erro ao ligar-se a uma Memoria_Contador");
 */
	if(CountSHM_id < 0){
	   	CountSHM_id = shmget( IPC_KEYB, sizeof(Contador) , IPC_CREAT | 0600 );
		print_on_error(CountSHM_id, "Erro ao criar uma Memoria_Contador");
 
 		Contador *count = (Contador *)shmat(CountSHM_id, 0 , 0);
		print_on_null(count, "Erro ao inicializar o ponteiro count(LINHA 18)");

		int sem_status = semop(semC_id, &DOWN, 1);
		print_on_error(sem_status, "DOWN_C(liga_Count)");

		count[0].cont_1 = 0;
		count->cont_2 = 0;
		count->cont_3 = 0;
		count->cont_perdida = 0;

		sem_status = semop(semC_id, &UP, 1);
		print_on_error(sem_status, "UP_C(liga_Count)");
	}

}


void cria_Memoria(){
	shm_id = shmget( IPC_KEY, NCONSULTAS * sizeof(Consulta) , 0);
    if (shm_id < 0){
    	shm_id = shmget( IPC_KEY, NCONSULTAS * sizeof(Consulta) , IPC_CREAT | 0600 );
    	print_on_error(shm_id, "Erro ao criar uma Memoria partilhada");
     }
    Consulta *mem = (Consulta *)shmat(shm_id, 0 , 0);
    print_on_null(mem, "shmat(cria_Memoria)");
 
    int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(cria_Memoria)");
	
	for(int i = 0; i < NCONSULTAS; i++){
    	mem[i].tipo = -1;
    	strcpy(mem[i].descricao, "Vazio");
    	mem[i].pid_consulta = -1;
    	mem[i].status = -1;
    }

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(cria_Memoria)");
}

int tem_vaga(Consulta *mem){ 
	for(int i = 0; i < NCONSULTAS; i++){
		if(mem[i].tipo == -1)
			return i;
	}
	return -1;
}

void incrementa_Cont(int tipo){
	Contador *c = (Contador *)shmat(CountSHM_id, 0, 0);
	print_on_null(c,"shmat(aceitar)");

	if(tipo == 1)
		c->cont_1++;
	if(tipo == 2)
		c->cont_2++;
	if(tipo == 3)
		c->cont_3++;
}

void remove_shm(Consulta *mem){
	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(179)");
	
	mem[sala_consulta].tipo = -1;

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(184)"); 
}

void trata_SIGALRM(int n){
	dedicada.consulta.status = 3;
	int status = msgsnd(msg_id, &dedicada, sizeof(dedicada.consulta), 0);
	print_on_error(status, "Erro em mandar status = 3");
	

	Consulta* mem =  (Consulta *) shmat(shm_id, 0 , 0);
	print_on_null(mem, "shmat(194)");

	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(trata_SIGALRM)");

	mem[sala_consulta].status = 3;

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(trata_SIGALRM)"); 

	remove_shm(mem);
	/* printf("O filho[%d] vai mandar o sinal para o %d(trata_SIGALRM)/ pai = %d\n", getpid(), getppid(), pai);
	kill(pai, SIGUSR1);
	printf("Mandou o sinal(trata_SIGALRM)\n");
	printf("%d vai fzr exit\n", getpid()); 
	kill(pai, SIGCHLD);*/
	exit(1);
}

void cancela_memoria(Consulta *mem){
	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(202)");

	mem[sala_consulta].status = 5;
	mem[sala_consulta].tipo = -1;

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(208)");
}

void mem_iniciaConsulta(Consulta *mem){
//	printf("Vai fzr down no mem_iniciaConsulta [%d]\n ", getpid());
	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(mem_iniciaConsulta)");
/* 	printf("Dormiu[%d]\n", getpid());
	sleep(10);
	printf("Acordou[%d]\n", getpid()); */
	mem[sala_consulta].status = 2;

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(mem_iniciaConsulta)");
//	printf("fez up no mem_iniciaConsulta [%d]\n", getpid());
}

void aceitar(Consulta *mem){
	int tipo = dedicada.consulta.tipo;
	int pid_consulta = dedicada.tipo;
//	printf("pid_consulta(222) = %d\n", pid_consulta);
	incrementa_Cont(tipo);

	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(aceitar)");
	
	mem[sala_consulta] = dedicada.consulta;

	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(aceitar)");

	dedicada.consulta.status = 2;
	
	int status = msgsnd(msg_id, &dedicada, sizeof(dedicada.consulta), 0);
	print_on_error(status, "Erro em mandar status = 2");
	
	mem_iniciaConsulta(mem);

	signal(SIGALRM, trata_SIGALRM);
//	printf("Vai alarmar\n");
	alarm(10);
//	printf("Alarmou\n");
	Mensagem m2;
	while(1){
//		printf("Ta a espera do cancela\n");
		status = msgrcv(msg_id, &m2, sizeof(m2.consulta), pid_consulta, 0);
		print_on_error(status, "erro ao receber(espera_Cancelamento)");
		if(m2.consulta.status == 5){
			printf("Pedido cancelado para o cliente[%d]\n", pid_consulta);
			cancela_memoria(mem);
			/* printf("Vai mandar o sinal(aceitar)\n");
			kill(getppid(), SIGUSR1);
			printf("Mandou o sinal(aceitar)\n"); */
			exit(0);
		}
	}
}

void recusar(){
	printf("Lista de consultas cheia\n");
	dedicada.consulta.status = 4;
	dedicada.tipo = dedicada.consulta.pid_consulta;

	int status = msgsnd(msg_id, &dedicada, sizeof(dedicada.consulta), 0);
	print_on_error(status, "Erro em mandar a Mensagem");
	
	Contador* cont_mem =  (Contador *) shmat(CountSHM_id, 0 , 0);
	print_on_null(cont_mem, "Erro ao escrever em cont_mem(246)")

	int sem_status = semop(semC_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_C(recusar)");
	
	cont_mem[0].cont_perdida++;

	sem_status = semop(semC_id, &UP, 1);
	print_on_error(sem_status, "UP_C(recusar)");	

//	kill(getppid(), SIGUSR1); 
	exit(0);
}

void manda_resposta(Consulta c){
	Consulta* mem =  (Consulta *) shmat(shm_id, 0 , 0);
	print_on_null(mem, "shmat");

	int sem_status = semop(semM_id, &DOWN, 1);
	print_on_error(sem_status, "DOWN_M(manda_resposta)");
	
	sala_consulta = tem_vaga(mem);
	
	sem_status = semop(semM_id, &UP, 1);
	print_on_error(sem_status, "UP_M(manda_resposta)");

	dedicada.consulta = c;
    dedicada.tipo = c.pid_consulta;
	if(sala_consulta == -1){
		recusar();
	}else{
		aceitar(mem);
	}
}

void cria_FilaMensagem(){
	msg_id = msgget(IPC_KEY, 0);
	if(msg_id < 0){
		msg_id = msgget(IPC_KEY, 0600 | IPC_CREAT | IPC_EXCL);	
		print_on_error(msg_id, "Erro no msgget.");
	}
}

void init(){
	int status; 
	Mensagem m;
	while(1){ 
		status = msgrcv(msg_id, &m, sizeof(m.consulta), 1, 0);
		print_on_error(status, "erro ao receber(MAIN)");

		cliente++;
//		printf("clientes = %d\n",cliente);

		printf("Chegou novo pedido de consulta do tipo <%d>, descrição <%s> e PID <%d>\n",m.consulta.tipo, m.consulta.descricao,m.consulta.pid_consulta);
		int filho = fork();
		if(!filho){
			printf("Novo servidor dedicado[%d]   --->   Cliente[%d]\n", getpid(), m.consulta.pid_consulta);
			manda_resposta(m.consulta);
		}
		else{
			signal(SIGCHLD, kill_Zombies);
		}
	}
}

int main(){
	printf("Servidor Clinica-IUL [%d]\n",getpid());

	signal(SIGINT, trata_SIGINT);
//	signal(SIGUSR1, trata_SIGUSR1);
//	signal(SIGCHLD, kill_Zombies);
	
	cria_sem();
//	inicia_filhos();
	liga_Count();
	cria_Memoria();
	cria_FilaMensagem();
	
	pai = getpid();
	cliente = 0;

	init();
}