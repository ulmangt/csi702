all: filter_serial

filter_serial: filter_io.o obs_math.o filter_math.o filter_serial.o
	gcc -I hdr -lm src/serial/filter_serial.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o -o bin/filter_serial

filter_serial.o:
	gcc -I hdr -c -lm src/serial/filter_serial.c -o src/serial/filter_serial.o

filter_math.o:
	gcc -I hdr -c -lm src/common/filter_math.c -o src/common/filter_math.o

obs_math.o:
	gcc -I hdr -c -lm src/common/obs_math.c -o src/common/obs_math.o

filter_io.o:
	gcc -I hdr -c -lm src/common/filter_io.c -o src/common/filter_io.o

clean:
	rm bin/filter_serial src/serial/filter_serial.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o

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
