CXX = g++
CXXFLAGS=-std=c++11
PARRFLAGS= -fopenmp -ltbb
VISFLAGS= -lGL -lGLU -lglut

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

ifeq ($(detected_OS),Darwin)  # Mac OS X
    VISFLAGS= -framework OpenGL -framework GLUT
endif


all: fluidanimate_no_vis fluidanimate_vis

fluidanimate_no_vis: serial.cpp cellpool.cpp fluid.hpp cellpool.hpp
	$(CXX) $(CXXFLAGS) $(PARRFLAGS) -o fluidanimate_no_vis serial.cpp cellpool.cpp

fluidanimate_vis: fluidview.cpp cellpool.cpp serial.cpp fluid.hpp cellpool.hpp fluidview.hpp
	$(CXX) $(CXXFLAGS) $(VISFLAGS) -o fluidanimate_vis -DENABLE_VISUALIZATION serial.cpp cellpool.cpp fluidview.cpp

clean:
	$(RM) *.o fluidanimate_no_vis fluidanimate_vis
