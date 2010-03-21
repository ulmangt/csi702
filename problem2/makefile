# -pg necessary for using gprof
# -lm necessary for math functions
problem2: problem2.o
	gcc -O1 -lm -pg problem2.o -o problem2

problem2.o: problem2.c
	gcc -O1 -c -pg problem2.c 

clean:
	rm problem2 problem2.o
