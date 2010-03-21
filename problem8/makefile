# -pg necessary for using gprof
# -lm necessary for math functions
problem8: problem8.o
	gcc -O2 -lm -pg problem8.o -o problem8

problem8.o: problem8.c
	gcc -O2 -c -pg problem8.c 

clean:
	rm problem8 problem8.o
