all: mutex_barrier condition_barrier

condition_barrier: condition_barrier.o
	gcc -I hdr -lm -lpthread src/condition_barrier.o -o bin/condition_barrier

condition_barrier.o:
	gcc -I hdr -c src/condition_barrier.c -o src/condition_barrier.o

mutex_barrier: mutex_barrier.o
	gcc -I hdr -lm -lpthread src/mutex_barrier.o -o bin/mutex_barrier

mutex_barrier.o:
	gcc -I hdr -c src/mutex_barrier.c -o src/mutex_barrier.o

clean:
	rm bin/mutex_barrier src/mutex_barrier.o 
