
sortv1.o: 
	gcc -c -o sortv1.o sortv1.c

test_sortv1.o:
	gcc -c -o test_sortv1.o test_sortv1.c -I..

test_sortv1: sortv1.o test_sortv1.o test_sortv1.c
	gcc -o test_sortv1 test_sortv1.o sortv1.o


quicksort_mt.o:
	gcc -c -o quicksort_mt.o quicksort_mt.c

test_quicksort_mt.o:
	gcc -c -o test_quicksort_mt.o test_quicksort_mt.c

test_quicksort_mt: sortv1.o quicksort_mt.o test_quicksort_mt.o
	gcc -o test_quicksort_mt sortv1.o quicksort_mt.o test_quicksort_mt.o -lpthread

clean:
	rm *.o
