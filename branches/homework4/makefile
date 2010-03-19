all: nbody_serial nbody_gen_particles

nbody_serial: nbody_serial.o
	gcc -lm src/problem3/nbody_serial.o -o bin/nbody_serial

nbody_serial.o:
	gcc -c src/problem3/nbody_serial.c -o src/problem3/nbody_serial.o

nbody_gen_particles: nbody_gen_particles.o
	gcc -lm src/problem3/nbody_gen_particles.o -o bin/nbody_gen_particles

nbody_gen_particles.o:
	gcc -c src/problem3/nbody_gen_particles.c -o src/problem3/nbody_gen_particles.o

clean:
	rm bin/nbody_serial src/problem3/nbody_serial.o bin/nbody_gen_particles src/problem3/nbody_gen_particles.o
