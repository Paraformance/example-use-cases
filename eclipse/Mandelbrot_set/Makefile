all: *.cpp
	c++ -g -std=c++1y -O3 -o mandelbrot save_image.cpp utils.cpp mandel.cpp -lfreeimage -ltbb -fopenmp

clean:
	rm -f mandelbrot *.o

dist-clean: clean
	rm -f mandelbrot.png
