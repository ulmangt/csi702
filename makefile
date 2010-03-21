mandel: mandel.o
	gcc -lm -lpthread mandel.o -o mandel

mandel.o: mandel.c
	gcc -c mandel.c

clean:
	rm mandel mandel.o
