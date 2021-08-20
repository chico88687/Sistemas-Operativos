#include "include.h"
#include "functions.c"


#define N_CONSULTAS 10

Consulta lista_consultas[N_CONSULTAS];
Consulta* lista_pointer = lista_consultas;

int count_1 = 0;
int *pCount_1 = &count_1;

int count_2 = 0;
int *pCount_2 = &count_2;

int count_3 = 0;
int *pCount_3 = &count_3;

int consulta_perdida = 0;
int *pConsulta_Perdida = &consulta_perdida;

void mostra_lista() {
    for(int i = 0; i < N_CONSULTAS; i++)
        printf("lista_consultas[%d] -> Tipo: %d & Descricao:%s & PID = %d\n", i, lista_consultas[i].tipo,lista_consultas[i].descricao, lista_consultas[i].pid_consulta);
}
	
void cria_ListaConsulta(Consulta *lista_pointer){
    for(int i = 0; i < N_CONSULTAS; i++){
        Consulta c;
//		c.tipo = 1;
        c.tipo = -1;
        lista_pointer[i] = c;
    }
}fseek

int regista_pid(){
    FILE* ft = fopen("SrvConsultas.pid","w");
    if(ft){
        fprintf(ft, "%d\n",getpid());
        fclose(ft);
		return 1;
	}
    else{
		printf("Não foi possível abri o ficheiro SrvConsultas.pid\n");
		return 0;
	}
} 

int tem_vaga(){
	for(int i = 0; i < N_CONSULTAS; i++){
		if (lista_consultas[i].tipo == -1){
//			printf("%s\n","Tem vaga!" );
			return i;
		}
	}
	return -1;
}

int cria_filho(int sala){
	if(fork() == 0){
//		sleep(10);
		kill(lista_consultas[sala].pid_consulta,SIGHUP);
		sleep(10);
		kill(lista_consultas[sala].pid_consulta,SIGTERM);
		exit(-1);
	}
	else{
		wait(NULL);
//		printf("sala no pai = %d\n", sala);	
		lista_consultas[sala].tipo = -1;
//		printf("%s\n","3a lista" );
//		mostra_lista();
	}
}

int insere_Consulta(int sala, char split_array[][SIZE]){
	printf("Consulta agendada para a sala <%d>\n", sala);
//	printf("%s,%s,%s\n",split_array[0],split_array[1],split_array[2]);
	int t = atoi(split_array[0]);
	int p = atoi(split_array[2]);
	lista_consultas[sala].tipo = t;
	strcpy(lista_consultas[sala].descricao, split_array[1]);
	lista_consultas[sala].pid_consulta = p;	
//	printf("%s\n","2a lista" );
//	mostra_lista();
	if(lista_consultas[sala].tipo == 1)
		count_1++;
	if(lista_consultas[sala].tipo == 2)
		count_2++;
	if(lista_consultas[sala].tipo == 3)
		count_3++;
	cria_filho(sala);	
}

void trata_SIGUSR1(int n){
//	printf("Entrou no trata\n");
	FILE* ft = fopen("PedidoConsulta.txt", "r");
	if(ft){
//		printf("Entrou no ft\n");
		char linha[SIZE];
		fgets(linha, SIZE, ft);
		fix_fgets(linha, strlen(linha));
		char* split;
		char split_array[3][SIZE];
		int i = 0;
		split = strtok(linha, ",");
		strcpy(split_array[i], split);
//		printf("%s\n", split_array[i]);
//		printf("%s\n", split);
		i++;
		while (/*split != NULL*/ i < 3){
//			printf("i = %d\n", i);
			split = strtok(NULL, ",");
			strcpy(split_array[i], split);
//			printf("%s\n", split_array[i]);
			i++;
		}
		printf("Chegou novo pedido de consulta do tipo <%s>, descrição <%s> e PID <%s>\n",split_array[0],split_array[1], split_array[2]);
		int vaga = tem_vaga();
//		printf("vaga()= %d\n",vaga);
		if(vaga >= 0)
			insere_Consulta(vaga, split_array);
		else{
			printf("%s\n","Lista de consulta cheia!" );
			consulta_perdida++;
			int pid_consulta = atoi(split_array[2]);
			kill(pid_consulta, SIGUSR2);
		}
	}else{
		printf("%s\n", "Não foi possivel abrir o ficheiro PedidoConsulta.txt");
	}
}

void read_dat(){
	FILE* fb = fopen("StatsConsulta.dat", "r");
	if(fb){
//		printf("entrou no fb_r\n");
		int var[4];
		int temp;
		int i = 0;
		while(fread(&temp, sizeof(int), 1, fb)){
			var[i] = temp;
			i++;
		}
		fclose(fb);
		consulta_perdida = var[0];
		count_1 = var[1];
		count_2 = var[2];
		count_3 = var[3];
	}else{
		printf("Não conseguiu ler o ficheiro StatsConsulta.dat\n");
	}
}

void write_dat(){
	FILE* fb = fopen("StatsConsulta.dat", "w");
	if(fb){
//		printf("entrou no fb_w\n");
		fwrite(pConsulta_Perdida, sizeof(int), 1, fb);
		fwrite(pCount_1, sizeof(int), 1, fb);
		fwrite(pCount_2, sizeof(int), 1, fb);
		fwrite(pCount_3, sizeof(int), 1, fb);
		fclose(fb);
	}else{
		printf("Não foi possível escrever no ficheiro StatsConsulta.dat\n");
	}
}

int remove_SrvConsultas(){
	if(remove("SrvConsultas.pid")==0){
		return 1;
	}else{
		return 0;
	}
}

void trata_SIGINT(int n){
	printf(" \n");
	write_dat();
	remove_SrvConsultas();		
	exit(-1);	
}

int main(){
	printf("Servidor Clinica-IUL [%d]\n",getpid());
	read_dat();
    cria_ListaConsulta(lista_pointer);
//	printf("%s\n","1a lista" );
//	mostra_lista();
	
	regista_pid();
    signal(SIGUSR1, trata_SIGUSR1);
	signal(SIGINT, trata_SIGINT);


	while(1){
		printf("Em espera...\n");
		pause();	
	}
}	

