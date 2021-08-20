#include "includes.h"

int main(){
	int id = shmget( IPC_KEY, NCONSULTAS * sizeof(Consulta) , 0 );
    exit_on_error(id, "shmget");

    Consulta* a = (Consulta *) shmat(id, NULL , 0);
    exit_on_null(a, "shmat");

    for( int i= 0; i < NCONSULTAS; i++) {
        printf("Pos %d: <%d> <%s> <%d> <%d>\n", i, a[i].tipo, a[i].descricao, a[i].pid_consulta,a[i].status);
    }
}
