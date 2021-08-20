#include "include.h"
#include "functions.c"

Consulta consulta;

int pid_Srv;

int recebeu_SIGHUP = 0;
int termina = 0;

int alarmou = 0;

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

int pede_Tipo(){
    int tipo;
	char s_tipo[SIZE];
	int aux = 0;
	int t = 0;
	while(!tipo_aceitar(s_tipo, t)){
		if(aux != 0)
			printf("Dados inválidos\n");
		printf("Tipo de Consulta: 1-Normal, 2-COVID19, 3-Urgente\n");
		printf("%s", "Insira o tipo de consulta: ");
		fgets(s_tipo, SIZE, stdin);
		fix_fgets(s_tipo, strlen(s_tipo));
		t = strlen(s_tipo);
		aux++;
	}
	tipo = atoi(s_tipo);
//  scanf("%d", &tipo);
//	getchar();
    return tipo;
}

void pede_Descricao(Consulta* c){
	char des[SIZE]; 
	printf("%s","Insira a descrição dao consulta: " );
	fgets(des,SIZE,stdin);
//	scanf("%s", &des);
	int size = strlen(des);
	fix_fgets(des, size);
	if(!strcmp(des,""))
		strcpy(des,"NONE");
	strcpy(c->descricao, des);
}

void inicia_Consulta(){
    consulta.tipo = pede_Tipo();
    pede_Descricao(&consulta);
    consulta.pid_consulta = getpid();
}

void cria_PedidoConsulta(int n){
	if(access("PedidoConsulta.txt", F_OK)){
		if(alarmou)
			printf("\n");
		FILE* ft = fopen("PedidoConsulta.txt","w");
		if(ft){
			fprintf(ft, "%d,%s,%d\n",consulta.tipo, consulta.descricao, consulta.pid_consulta);
			fclose(ft);
		}
		else{
			printf("%s\n","Não foi possivel abrir o ficheiro PedidoConsulta.txt");
		}
		kill(pid_Srv, SIGUSR1);
	}else{
		if(!alarmou){
			printf("Aguarde...");
			fflush(stdout);
			alarmou++;
		}else{
			printf(".");
			fflush(stdout);
		}
		alarm(10);
	}
}

int servidor_On(){
	if(access("SrvConsultas.pid", F_OK)){
		return 0;
	}
	else
		return 1;
}

int pid_SrvConsultas(){
	if(servidor_On()){
		FILE* ft = fopen("SrvConsultas.pid", "r");
		if(ft){
			char buffer[256];
			fgets(buffer, 256, ft);
			fix_fgets(buffer, strlen(buffer));
//			printf("%s\n",buffer );
			pid_Srv = atoi(buffer);
//			printf("pid = %d\n", pid_Srv);
			fclose(ft);
			return 0;
		}
		else{
			printf("%s\n","Não foi possivel abrir o ficheiro SrvConsultas.pid" );
			return 1;
		}
	}else{
		printf("O servidor Clinica-IUL não se encontra ativo, por favor tente mais tarde.\n");
		exit(0);	
	}
}

/*void send_SIGUSR1(){
	kill(pid_Srv, SIGUSR1);	
}*/

int remove_PedidoConsulta(){
	if(remove("PedidoConsulta.txt") == 0){
//		printf("\nPedidoConsulta.txt deletado com sucesso\n");
		return 1;
	}
	else{
//		printf("\nNão foi possível deletar o ficheiro PedidoConsulta.txt\n"); 
		return 0;
	}
}

void trata_SIGHUP(int n){
	recebeu_SIGHUP = 1;
	printf("Consulta iniciada para o processo <%d>\n", getpid());
	remove_PedidoConsulta();
}

void trata_SIGTERM(int n){
	if(recebeu_SIGHUP){
		printf("Consulta concluída para o processo <%d>\n", getpid());
		termina = 1;
	}
	else{
		printf("Recebeu um SIGTERM sem ter recebido um SIGHUP");
		exit(-1);
	}
}

void trata_SIGUSR2(int n){
	printf("Consulta não é possível para o processo <%d>\n", getpid());	
	remove_PedidoConsulta();
	termina = 1;
}

void trata_SIGINT(int n){
	remove_PedidoConsulta();
	printf("\n%s\n","Paciente cancelou o pedido");
	exit(-1);
}

int main(){
	signal(SIGHUP, trata_SIGHUP);
    signal(SIGTERM, trata_SIGTERM);
    signal(SIGUSR2, trata_SIGUSR2);
    signal(SIGINT, trata_SIGINT);
    signal(SIGALRM, cria_PedidoConsulta);

	printf("Cliente Clínica-IUL[%d]\n", getpid());	
	pid_SrvConsultas();
	printf("Servidor[%d]\n",pid_Srv);	
    inicia_Consulta();
	
	printf("Tipo: %d\nDescrição: %s\nPID: %d\n", consulta.tipo, consulta.descricao, consulta.pid_consulta);

	cria_PedidoConsulta(0);
//	kill(pid_Srv, SIGUSR1);

	while(!termina)
		pause();
}
