

all: filter_serial filter_cuda

clean: clean_cuda clean_serial


#
# CUDA Code
#

filter_cuda: filter_cuda_kernel.o filter_cuda.o filter_math.o obs_math.o filter_io.o convert.o filter_cuda_util.o
	nvcc -I hdr -lcuda -lcudart -lm src/cuda/filter_cuda_util.o src/cuda/filter_cuda_kernel.o src/cuda/filter_cuda.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o src/common/convert.o -o bin/filter_cuda

filter_cuda.o:
	gcc -I hdr -c src/cuda/filter_cuda.c -o src/cuda/filter_cuda.o

filter_cuda_util.o:
	gcc -I hdr -c src/cuda/filter_cuda_util.c -o src/cuda/filter_cuda_util.o

filter_cuda_kernel.o:
	nvcc -I hdr -c src/cuda/filter_cuda_kernel.cu -o src/cuda/filter_cuda_kernel.o

clean_cuda:
	rm bin/filter_cuda src/cuda/filter_cuda_util.o src/cuda/filter_cuda_kernel.o src/cuda/filter_cuda.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o src/common/convert.o

#
# Serial Code
#

filter_serial: filter_io.o obs_math.o filter_math.o filter_serial.o convert.o
	gcc -I hdr -lm src/serial/filter_serial.o src/common/convert.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o -o bin/filter_serial

filter_serial.o:
	gcc -I hdr -c -lm src/serial/filter_serial.c -o src/serial/filter_serial.o

filter_math.o:
	gcc -I hdr -c -lm src/common/filter_math.c -o src/common/filter_math.o

obs_math.o:
	gcc -I hdr -c -lm src/common/obs_math.c -o src/common/obs_math.o

filter_io.o:
	gcc -I hdr -c -lm src/common/filter_io.c -o src/common/filter_io.o

convert.o:
	gcc -I hdr -c -lm src/common/convert.c -o src/common/convert.o

clean_serial:
	rm bin/filter_serial src/serial/filter_serial.o src/common/filter_math.o src/common/obs_math.o src/common/filter_io.o src/common/convert.o


#
# Documentation
#

docs:
	pdflatex -output-directory docs report.tex

clean_docs:
	rm docs/report.aux docs/report.log docs/report.pdf docs/report.toc

#
# CDUA SDK Examples
#

examples: cudaMallocAndMemcpy myFirstKernel reverseArray_singleblock reverseArray_multiblock

reverseArray_multiblock:
	nvcc src/examples/reverseArray_multiblock.cu -o bin/reverseArray_multiblock
	
reverseArray_singleblock:
	nvcc src/examples/reverseArray_singleblock.cu -o bin/reverseArray_singleblock

myFirstKernel:
	nvcc src/examples/myFirstKernel.cu -o bin/myFirstKernel

cudaMallocAndMemcpy:
	nvcc src/examples/cudaMallocAndMemcpy.cu -o bin/cudaMallocAndMemcpy

clean_examples:
	rm bin/cudaMallocAndMemcpy bin/myFirstKernel bin/reverseArray_singleblock bin/reverseArray_multiblock


#
# Other Timing Tests
#

scan_sum_test: filter_cuda_kernel.o
	nvcc -I hdr -lcuda -lcudart -lm src/cuda/filter_cuda_kernel.o  src/cuda/scan_sum_test.cu -o bin/scan_sum_test

#scan_sum_test: filter_cuda_kernel.o scan_sum_test.o
#	nvcc -I hdr -lcuda -lcudart -lm src/cuda/filter_cuda_kernel.o -o bin/scan_sum_test

#scan_sum_test.o:
#	nvcc -I hdr -c src/cuda/scan_sum_test.cu -o src/cuda/scan_sum_test.o
