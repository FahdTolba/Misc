#ifndef __MT_QUICKSORT_H
#define __MT_QUICKSORT_H

#define MIN(A,B) A >= B ? A : B

#define MAX_SIZE 100000000 //make sure you have at least 4GB RAM to hold this much data
				//otherwise you'll be getting a segmentation fault on process execution
#define DEF_ALT S
#define DEF_THR 10
#define DEF_MT Y
#define DEF_PIECES 500
#define DEF_MAXTHRD 4
#define	DEF_MEDIAN N

struct segment{
	int *arr;
	int lo;
	int hi;
};

#endif
