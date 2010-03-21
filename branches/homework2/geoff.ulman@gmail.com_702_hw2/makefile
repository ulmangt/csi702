all: mandelbrot_threaded mandelbrot_serial

mandelbrot_threaded: mandelbrot_threaded.o
	gcc -lm -lpthread mandelbrot_threaded.o -o mandelbrot_threaded

mandelbrot_threaded.o: mandelbrot_threaded.c
	gcc -c mandelbrot_threaded.c

mandelbrot_serial: mandelbrot_serial.o
	gcc -lm -lpthread mandelbrot_serial.o -o mandelbrot_serial

mandelbrot_serial.o: mandelbrot_serial.c
	gcc -c mandelbrot_serial.c

mandelbrot_threaded_alternate: mandelbrot_threaded_alternate.o
	gcc -lm -lpthread mandelbrot_threaded_alternate.o -o mandelbrot_threaded_alternate

mandelbrot_threaded_alternate.o: mandelbrot_threaded_alternate.c
	gcc -c mandelbrot_threaded_alternate.c

clean:
	rm mandelbrot_serial mandelbrot_serial.o mandelbrot_threaded mandelbrot_threaded.o

clean_alternate:
	rm mandelbrot_threaded_alternate mandelbrot_threaded_alternate.o

