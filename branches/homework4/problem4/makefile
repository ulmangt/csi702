all: sort_parallel sort_serial 

run_parallel: sort_parallel
	mpirun -np 10 -hostfile hostfile bin/sort_parallel

run_serial: sort_serial
	bin/sort_serial

sort_parallel: sort_parallel.o sort_util.o
	mpicc -I hdr -lm src/sort_parallel.o src/sort_util.o -o bin/sort_parallel

sort_parallel.o:
	mpicc -I hdr -c src/sort_parallel.c -o src/sort_parallel.o

sort_serial: sort_serial.o sort_util.o
	gcc -I hdr -lm src/sort_serial.o src/sort_util.o -o bin/sort_serial

sort_serial.o:
	gcc -I hdr -c src/sort_serial.c -o src/sort_serial.o

sort_util.o:
	gcc -I hdr -c -lm src/sort_util.c -o src/sort_util.o

clean:
	rm bin/sort_serial bin/sort_parallel src/sort_serial.o src/sort_parallel.o src/sort_util.o
