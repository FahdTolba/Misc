#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>

#include"quicksort_mt.h"

pthread_mutex_t thr_num_mtx;
int *A[MAX_SIZE];

int SIZE = -1;
char ALT = 'DEF_ALT';

int THRESHOLD = DEF_THR; //size of array to use alternate sorting algorithm on

int SEED;
char seed_flg = 0;      //determine whether a seed option was provided

char MULTITHREAD = 'DEF_MT';

int PIECES = DEF_PIECES;        //number of segments to divide original array into
int MAXTHREADS = DEF_MAXTHRD;
char MEDIAN = 'DEF_MEDIAN';

#define calc_time_diff(start, end, result)                      \
        if(start.tv_usec > end.tv_usec){                        \
                end.tv_usec += 1000000;                         \
                end.tv_sec--;                                   \
        }                                                       \
                                                                \
        result.tv_usec = end.tv_usec - start.tv_usec;           \
        result.tv_sec = end.tv_sec - start.tv_sec


timeval_to_float_sec(struct timeval *tvptr, double *result){
	*result = tvptr->tv_sec;
	*result += (float)tvptr->tv_usec/1000000;
}

get_args(int argc, char *argv[]){

int c;
int temp;


	while((c = getopt(argc, argv,
			"n:a:s:r:m:p:t:e:")) != -1){

		temp = 0;
		switch(c){
			case 'n':
				SIZE = atoi(optarg);
				if(SIZE < 1
				|| SIZE > MAX_SIZE){
					printf("invalid SIZE argument: %d\n", SIZE);
					printf("valid range [1..1000000000] (1 to 1 billion inclusive)\n");
					exit(1);
				}
				break;
			case 's':
				temp = atoi(optarg);
				if(temp < 3){
					printf("invalid THRESHOLD value %d, minimum is 3\n", THRESHOLD);
					printf("using default value: DEF_THR\n");
				}else
					THRESHOLD = temp;				
				break;
			case 'r':
				SEED = atoi(optarg);
				seed_flg = 1;
				break;
			case 'm': //printf("m\n");
				if(optarg[0] != 'Y'
				&& optarg[0] != 'y'
				&& optarg[0] != 'N'
				&& optarg[0] != 'n'){
					printf("invalid value for MULTITHREADED option: %c\n", optarg);
					printf("using default DEF_MT\n");
				}else
					MULTITHREAD = optarg[0];
				break;
			case 'p':
				temp = atoi(optarg);
				if(temp < 1){
					printf("PIECES must be a positive integer\n");
					printf("using default value DEF_PIECES\n");
				}else
					PIECES = temp;
				break;
			case 't':
				temp = atoi(optarg);
				if(temp < 1){
					printf("MAXTHREADS must be a positive integer\n");
					printf("using default value DEF_MAXTHRD\n");
				}else
					MAXTHREADS = temp;
				break;
			case 8: //median (m3)
				if(optarg != 'Y'
				&& optarg != 'y'
				&& optarg != 'N'
				&& optarg != 'n'){
					printf("invalid value for MEDIAN: %c\n", optarg);
					printf("using default value DEF_MEDIAN\n");
				}else
					MEDIAN = optarg;
				break;
		}
	}

	if(SIZE == -1) {
		printf("no SIZE argument provided\n");
		exit(1);
	}

//printf("c is %d\n", c);
}


get_input(char *fname, int **A){ printf("get_input() \n");

int fd, offset;
int n, count, i;
struct timeval tv;

	fd = open(fname, O_RDONLY);
	if(fd < 0){
		printf("open(\"%s\") error: %s\n",fname, strerror(errno));
		exit(1);
	}
	if(seed_flg){
		offset = SEED;
	}else{
		gettimeofday(&tv, NULL);
		srand(tv.tv_usec);
		offset = rand() % MAX_SIZE;
	}

	if(offset > SIZE){//printf("offset is greater than SEED\n");
		offset %= SIZE;
	}
printf("get_input() midpoint offset %d\n",offset);
	//read data from file
	count = SIZE - offset;
	lseek(fd, offset * sizeof(int), SEEK_SET);printf("lseek()\n");
	for(i = 0; i < count; i++){ //printf("read integer\b");
		n = read(fd, A[i], sizeof(int));
	}

printf("first read from file %d bytes\n", n);

	count = SIZE - count;
	lseek(fd, 0, SEEK_SET);
	for(i = 0; i < count; i++){
		n = read(fd, A[i], sizeof(int));
	}
//printf("Second read from file %d bytes\n", n);
printf("get_input() end\n");
	close(fd);

}

void output_statistics
(struct timeval *load_start, struct timeval *load_end, struct timeval *wall_end){

struct timeval wall_time, load_time, total_time;
double load_time_result, wall_time_result, total_time_result;


	calc_time_diff((*load_start), (*load_end), load_time);

	calc_time_diff((*load_end), (*wall_end), wall_time);//actual program runtime (time taken by threads in parallel)

	clock_t clock_time = clock(); //sum of time taken by threads

	total_time.tv_sec = load_time.tv_sec + wall_time.tv_sec;
	total_time.tv_usec = load_time.tv_usec + wall_time.tv_usec;

	//total_time = load_time + wall_time
	if(total_time.tv_usec > 1000000){
		total_time.tv_sec++;
		total_time.tv_usec -= 1000000;
	}

	//load in seconds
	timeval_to_float_sec(&load_time, &load_time_result);

	//wall in seconds
	timeval_to_float_sec(&wall_time, &wall_time_result);

	double total_time_res; 		//result total time in seconds
	timeval_to_float_sec(&total_time, &total_time_result);	


	printf("Load: %.6g Sort (Wall/CPU): (%g / %g) Total: %g\n",
		load_time_result, wall_time_result, (double)clock_time/CLOCKS_PER_SEC, total_time_result);
}

int 
main(int argc, char *argv[]){

//printf("argv: %p\n", argv);

struct timeval load_start, load_end, wall_end;

unsigned int p, r, j;

	
	//get arguments (configuration)
	get_args(argc, argv);
printf("get_args() returned\n");
	gettimeofday(&load_start, NULL);

	for(j = 0; j < SIZE; j++) 
		A[j] = malloc(sizeof(int)); //actually using malloc would result in having 
                                                //random values in the list 

	//generate or read input
	get_input("random.dat", A);
	gettimeofday(&load_end, NULL);
printf("get_input() returned\n");
	p = 0;
	r = SIZE-1;

	while(PIECES > SIZE/2){
		PIECES /= 2;
	}


	if(PIECES < 2
	|| MAXTHREADS < 2){
		//printf("invoking randomized_quicksort(), no Multi-Threading\n");
		randomized_quicksort(A, p, r);

	}else{ //Multi-Threading Enabled

		//Divide array into segments

		//An array of segment structures
		struct segment *seg_arr = malloc(PIECES * sizeof(struct segment)); //allocate PIECES structures of segment
	
		partition_list(A, p, r, seg_arr, &PIECES);
printf("partition returned\n");
		start_threads(seg_arr, MIN(MAXTHREADS, PIECES));
printf("start_thr returned\n");
		printf("threads finished\n");		
	}//end if (Multi-Threading Enabled)	

	gettimeofday(&wall_end, NULL);

	output_statistics(&load_start, &load_end, &wall_end);

	//verify array is sorted
	if(!isSorted(A, SIZE))
		printf("ERROR - Data Not Sorted\n");
	else
		printf("SUCCESS, Data is sorted\n");

	//TODO: free dynamically allocated memory
}
