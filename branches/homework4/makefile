all: nbody_serial nbody_parallel nbody_gen_particles

run_parallel: nbody_parallel
	mmpirun -np 10 -hostfile hostfile bin/nbody_parallel

run_serial: nbody_serial
	bin/nbody_serial

nbody_parallel: nbody_parallel.o nbody_util.o
	mpicc -I hdr -lm src/problem3/nbody_parallel.o src/problem3/nbody_util.o -o bin/nbody_parallel

nbody_parallel.o:
	mpicc -I hdr -c src/problem3/nbody_parallel.c -o src/problem3/nbody_parallel.o

nbody_serial: nbody_serial.o nbody_util.o
	gcc -I hdr -lm src/problem3/nbody_serial.o src/problem3/nbody_util.o -o bin/nbody_serial

nbody_serial.o:
	gcc -I hdr -c src/problem3/nbody_serial.c -o src/problem3/nbody_serial.o

nbody_gen_particles: nbody_gen_particles.o nbody_util.o
	gcc -I hdr -lm src/problem3/nbody_gen_particles.o src/problem3/nbody_util.o -o bin/nbody_gen_particles

nbody_gen_particles.o:
	gcc -I hdr -c src/problem3/nbody_gen_particles.c -o src/problem3/nbody_gen_particles.o

nbody_util.o:
	gcc -I hdr -c -lm src/problem3/nbody_util.c -o src/problem3/nbody_util.o

clean:
	rm bin/nbody_serial src/problem3/nbody_serial.o bin/nbody_gen_particles src/problem3/nbody_gen_particles.o src/problem3/nbody_util.o
