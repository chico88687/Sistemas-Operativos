#include "include.h"

int main(){
	int var[4];
	FILE* fb = fopen("StatsConsulta.dat", "r");
    if(fb){
        int temp;
        int i = 0;
        while(fread(&temp, sizeof(int), 1, fb)){
            var[i] = temp;
            i++;
		}
		fclose(fb);
	}else{
		printf("Não conseguiu ler o ficheiro StatsConsulta.dat\n");
    }
	printf("Perdidas = %d\n", var[0]);
	printf("Normal  = %d\n", var[1]);
	printf("COVID-19 = %d\n", var[2]);
	printf("Urgentes = %d\n", var[3]);		
}
