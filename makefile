all: mutex_barrier

mutex_barrier: mutex_barrier.o
	gcc -I hdr -lm src/mutex_barrier.o -o bin/mutex_barrier

mutex_barrier.o:
	gcc -I hdr -c src/mutex_barrier.c -o src/mutex_barrier.o

clean:
	rm bin/mutex_barrier src/mutex_barrier.o 
