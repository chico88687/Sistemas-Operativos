#include "include.h"

void fix_fgets(char* string,int size){
    string[size - 1] = 0;
}

int isInteger(char *c){
	int t = 0;
	while(*c){
		if(isdigit(*c))
			c++;
		else
			return 0;
		t++;
	}
	if(t == 0)
		return 0;
	return 1;
}

