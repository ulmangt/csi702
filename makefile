all: filter_serial

filter_serial: filter_serial.o
	gcc src/serial/filter_serial.o -o bin/filter_serial

filter_serial.o:
	gcc -c -lm src/serial/filter_serial.c -o src/serial/filter_serial.o

clean:
	rm bin/filter_serial src/serial/filter_serial.o

examples: cudaMallocAndMemcpy myFirstKernel reverseArray_singleblock reverseArray_multiblock

reverseArray_multiblock:
	nvcc src/examples/reverseArray_multiblock.cu -o bin/reverseArray_multiblock
	
reverseArray_singleblock:
	nvcc src/examples/reverseArray_singleblock.cu -o bin/reverseArray_singleblock

myFirstKernel:
	nvcc src/examples/myFirstKernel.cu -o bin/myFirstKernel

cudaMallocAndMemcpy:
	nvcc src/examples/cudaMallocAndMemcpy.cu -o bin/cudaMallocAndMemcpy

report:
	pdflatex -output-directory docs report.tex

clean_examples:
	rm bin/cudaMallocAndMemcpy bin/myFirstKernel bin/reverseArray_singleblock bin/reverseArray_multiblock

clean-docs:
	rm docs/report.aux docs/report.log docs/report.pdf docs/report.toc
