#include<stdio.h>
#include<stdlib.h>


print_arr(int *A[], int n){

        for(int i = 0; i < n; i++){
                fprintf(stdout,"%d", *A[i]);
                printf("\n");
        }
}


main(){

#define MAX 20

int normal_integer;
int *A[MAX];
int max;
char tmp[MAX];
int i;

	printf("enter number of elements to be sorted desired number MAX: %d\n", MAX);
	scanf("%s", tmp); printf("a7a\n");


	max = atoi(tmp);
	printf("max is %d\n", max);

	//fill in array of integers
	for(i = 0; i < max; i++){
		scanf("%s", tmp);
		A[i] = malloc(sizeof(int));
		*A[i] = atoi(tmp);
	}

	//sort array
	randomized_quicksort(A, 0, max-1);

	printf("sorted list\n");
	//output result
	print_arr(A, max);
}
