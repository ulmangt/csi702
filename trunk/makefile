

all: filter_serial filter_cuda summation_test

clean: clean_cuda clean_serial clean_summation_test

clean_common:
	rm src/cuda/filter_cuda_kernel.o

#
# CUDA Code
#

filter_cuda: filter_cuda_kernel.o filter_cuda.o filter_math.o observation.o waypoint.o convert.o filter_cuda_util.o
	nvcc -I hdr -lcuda -lcudart -lm src/cuda/filter_cuda_util.o src/cuda/filter_cuda_kernel.o src/cuda/filter_cuda.o src/common/filter_math.o src/common/observation.o src/common/waypoint.o src/common/convert.o -o bin/filter_cuda

filter_cuda.o:
	gcc -I hdr -c src/cuda/filter_cuda.c -o src/cuda/filter_cuda.o

filter_cuda_util.o:
	gcc -I hdr -c src/cuda/filter_cuda_util.c -o src/cuda/filter_cuda_util.o

filter_cuda_kernel.o:
	nvcc -I hdr -lcuda -lcudart -lm -c src/cuda/filter_cuda_kernel.cu -o src/cuda/filter_cuda_kernel.o

clean_cuda: clean_common
	rm bin/filter_cuda src/cuda/filter_cuda_util.o src/cuda/filter_cuda.o

#
# Serial Code
#

filter_serial: waypoint.o observation.o filter_math.o filter_serial.o convert.o
	gcc -I hdr -lm src/serial/filter_serial.o src/common/convert.o src/common/filter_math.o src/common/observation.o src/common/waypoint.o -o bin/filter_serial

filter_serial.o:
	gcc -I hdr -c -lm src/serial/filter_serial.c -o src/serial/filter_serial.o

filter_math.o:
	gcc -I hdr -c -lm src/common/filter_math.c -o src/common/filter_math.o

observation.o:
	gcc -I hdr -c -lm src/common/observation.c -o src/common/observation.o

waypoint.o:
	gcc -I hdr -c -lm src/common/waypoint.c -o src/common/waypoint.o

convert.o:
	gcc -I hdr -c -lm src/common/convert.c -o src/common/convert.o

clean_serial:
	rm bin/filter_serial src/serial/filter_serial.o src/common/filter_math.o src/common/observation.o src/common/waypoint.o src/common/convert.o

#
# Documentation
#

docs:
	pdflatex -output-directory docs report.tex

clean_docs:
	rm docs/report.aux docs/report.log docs/report.pdf docs/report.toc


#
# Other Timing Tests
#

summation_test: summation_test_thrust.o summation_test.o filter_cuda_kernel.o
	nvcc -I hdr -lcuda -lcudart -lm src/cuda/filter_cuda_kernel.o src/cuda/summation_test_thrust.o  src/cuda/summation_test.o -o bin/summation_test

summation_test.o:
	gcc -I hdr -lm -c src/cuda/summation_test.c -o src/cuda/summation_test.o

summation_test_thrust.o:
	nvcc -I hdr -lcuda -lcudart -lm -c src/cuda/summation_test_thrust.cu -o src/cuda/summation_test_thrust.o

clean_summation_test: clean_common
	rm bin/summation_test src/cuda/summation_test.o src/cuda/summation_test_thrust.o

