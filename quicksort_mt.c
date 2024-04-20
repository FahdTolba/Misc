#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>
#include<errno.h>
#include<string.h>
#include<stdbool.h>

#include"quicksort_mt.h"

bool
isSorted(int **arr, int length){ //printf("isSorted(), arr size %d\n", length);
int i;
	for(i = 0; i < length - 1; i++){
	//printf("arr[%d]: %d  arr[%d]: %d\n", i, arr[i], i+1, arr[i+1]);
		if(*arr[i] > *arr[i+1])
			return false;
	}

	return true;
}

int
thread_run(void *arg){

	struct segment *thr_arg = (struct segment *)arg;

	int **A = thr_arg->arr;
	int lo = thr_arg->lo;
	int hi = thr_arg->hi;
	
	//printf("Launching thread to sort %d - %d (%d)\n", lo, hi, hi-lo+1);
	randomized_quicksort(A, lo, hi);

/*DBG
	if(isSorted(A, lo,hi))
		printf("thread sorting %d to %d finished\n", lo, hi);
	else
		printf("thread sorting %d to %d finished sorting incorrectly", lo,hi);
*/

	return 0;
}

void
partition_list
(int *A[], int p, int r, struct segment *seg_arr, int *pieces){ printf("partition start\n");

int seg_arr_idx = 0;
int max_idx;

//used for output
int size_old;
int tmp_lo_1, tmp_hi_1, size_1;
int tmp_lo_2, tmp_hi_2, size_2;


	if(r-p+1 < *pieces) return;


	//first element in segment array
	seg_arr[0].arr = A;
	seg_arr[0].lo = p;
	seg_arr[0].hi = r;

	while(seg_arr_idx+1 < *pieces){
		/*find and divide the largest segment*/
				
		//find largest segment
		max_idx = 0;
		int max = seg_arr[0].hi - seg_arr[0].lo;
		int temp;
	
		for(int i = 1; i <= seg_arr_idx; i++){
			temp = seg_arr[i].hi - seg_arr[i].lo;
			if(temp > max){
				max = temp;
				max_idx = i;
			}
		}

		if(max < 3){
			//cannot partition a segment with less
			//than 3 elements
			//cannot partition into PIECES segments,
			//	continuing with seg_arr_idx+1 segments
			*pieces = seg_arr_idx+1;
			break;
		}

		tmp_lo_1 = seg_arr[max_idx].lo;
		tmp_hi_1 = seg_arr[max_idx].hi;
		size_old = tmp_hi_1 - tmp_lo_1 + 1;

		//partition largest found segment
		int X, i, j, q;
		int lo = seg_arr[max_idx].lo;
		int hi = seg_arr[max_idx].hi;	 

	//printf("partitioning %d - %d (%d)",lo, hi, hi-lo+1);
		q = randomized_partition(A, lo, hi);

		//update boundaries
		//seg_arr[max_idx].lo = old_lo; // unchanged
		temp = seg_arr[max_idx].hi; //store to use for new entry
		seg_arr[max_idx].hi = q-1;

		//add new segment to seg_arr, and set its boundaries
		seg_arr_idx++;
		seg_arr[seg_arr_idx].arr = A;
		seg_arr[seg_arr_idx].lo = q+1;
		seg_arr[seg_arr_idx].hi = temp;	//old hi

		//usleep(100);

	}//while
}

//CHECK use a generic thread_function argument thread_start = thread_run;
void start_threads(struct segment *seg_arr, int max_threads){

int retval;
int *retval_ptr = &retval;
int thr_start = 0;
int thr_num = 0;

	pthread_t *tid_arr = malloc(max_threads * sizeof(pthread_t));    //array to hold thread id's

	thr_start = max_threads; //number of threads to start

	int k = 0;
	//start threads, MAXTHREADS thread running at a time
	while(1){//printf("while loop\n");
		if(thr_start < 1)	//no more segments to sort
			break;

		if(thr_num >= max_threads){ //printf("checking for free thread slots\n");
			k = 0;
			//for each running thread
			int avail = 0;
			for(k = 0; k < max_threads; k++){ //printf("for loop, running threads %d\n", thr_num);
				int ret;
				ret = pthread_tryjoin_np(tid_arr[k], &retval_ptr);
				if(ret == 0){ //printf("one thread finished\n");
					thr_num--;
					avail = 1; //start a new thread
					break;
				}
			}

			if(avail) goto launch_thread;


			//printf("no available slots found\n");
			usleep(100000);//sleep 100 milli-seconds before trying again
			continue;
		}

	launch_thread:
	//printf("starting thread at tid_arr[%d]\n", k);

		 //create new thread for next unsorted segment (from seg_arr)
		 // & decrement number of segments left to sort
		 pthread_create(&tid_arr[k++] , NULL, thread_run, &seg_arr[--thr_start]);
		 thr_num++; //increment number of running threads
	}//while()

	printf("waiting for threads to finish\n");

	//wait for remaining threads to finish
	for(int k = 0; k < thr_num; k++){
		pthread_join(tid_arr[k], &retval_ptr);
	}
}
