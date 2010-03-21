# -pg necessary for using gprof
# -lm necessary for math functions
problem5: problem5.o
	gcc -O2 -lm -pg problem5.o -o problem5

problem5.o: problem5.c
	gcc -O2 -c -pg problem5.c 

clean:
	rm problem5 problem5.o
