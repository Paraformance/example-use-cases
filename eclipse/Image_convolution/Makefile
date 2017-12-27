CXX = g++ 

FLAGS = -std=c++11
SEQ_LFLAGS = -ltbb -lpng16

PNG_INC = .
#LIBS = -L/usr/lib64


INCLUDE = -I$(PNG_INC)
LIBS = -L/usr/local/lib


all: seq

seq:
	$(CXX) -g $(FLAGS) $(LIBS) $(INCLUDE) convolution_seq.cpp -o convolution_seq $(SEQ_LFLAGS)

clean:
	rm -rf convolution_seq

