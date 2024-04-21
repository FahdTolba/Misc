#include"sort.h"

int
_random(int min, int max){

int j;
int range = max - min; //CHECK, might be off by one

	j = rand() % range;

        return j + min;
}


//randomize in place an array of pointers
void
randomize_inplace(int *A[], int p, int r){

void *temp;
int j;
int max = r + 1;

	srand(clock());

	for(int i = p; i < r; i++){

		j = _random(i, r);

		//A[i] <-> A[j];
		temp = A[i];
		A[i] = A[j];
		A[j] = temp;
	}
}

/*
partition(A, p, r)

	i = p - 1;

	for j <- p to r-1
		if A[j] <= A[r]
			i <- i + 1;
			A[i] <-> A[j]

	i <- i + 1;
	A[i] <-> A[r]
*/

//partition modified to work
//on an array of node ptrs
int
partition_ptr(int *A[], int p, int r){

int i = p-1;

void *temp;

	for(int j = p; j < r; j++){
		if(*A[j] <= *A[r]){
			i++;
			//A[i] <-> A[j]
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;
		}
	}
	
	i++;
	//A[i] <-> A[r]
	temp = A[i];
	A[i] = A[r];
	A[r] = temp;

	return i;
}

int
randomized_partition(int *A[], int p, int r, char rand_flag){

	if(rand_flag)
		randomize_inplace(A, p, r);

	return partition_ptr(A, p, r);
}

//randomized_quicksort, modified to use pointers
//to elements being sorted
// imp: r is the index of the last element not the number
//	of elements
void
randomized_quicksort(int *A[], int p, int r, char rand_flag){

int q;

	if(p >= r) {
		return;
	}
	q = randomized_partition(A, p, r, rand_flag);

	randomized_quicksort(A, p, q-1, 0);
	randomized_quicksort(A, q+1, r, 0);
}

