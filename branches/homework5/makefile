all: mandel_serial mandel_parallel

run_serial: mandel_serial
	bin/mandel_serial

run_parallel: mandel_parallel
	export OMP_NUM_THREADS=4
	export OMP_SCHEDULE=static,10
	bin/mandel_parallel

view_results:
	octave --persist mandel_view

mandel_serial: mandel_serial.o
	gcc src/mandel_serial.o -o bin/mandel_serial

mandel_serial.o:
	gcc -c src/mandel_serial.c -o src/mandel_serial.o

mandel_parallel: mandel_parallel.o
	gcc -fopenmp src/mandel_parallel.o -o bin/mandel_parallel

mandel_parallel.o:
	gcc -fopenmp -c src/mandel_parallel.c -o src/mandel_parallel.o

clean:
	rm src/mandel_serial.o bin/mandel_serial src/mandel_parallel.o bin/mandel_parallel
