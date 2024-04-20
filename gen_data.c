#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#define MAX_INT 1000000
#define MAX_NUM	999999999

main(){
int x;
	srand(clock());

	FILE *fp = fopen("random.dat","w+");

	//generate random integers
	for(int i; i < MAX_INT; i++){
		x = rand() % MAX_INT;
		fwrite(&x, 1, 4, fp);
	}
	fclose(fp);
}
