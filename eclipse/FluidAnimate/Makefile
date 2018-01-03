CXXFLAGS=-std=c++11

all: fluidanimate fluidanimate_vis

fluidanimate: serial.cpp cellpool.cpp fluid.hpp cellpool.hpp
	$(CXX) $(CXXFLAGS) -o fluidanimate serial.cpp cellpool.cpp

fluidanimate_vis: fluidview.cpp cellpool.cpp serial.cpp fluid.hpp cellpool.hpp fluidview.hpp
	$(CXX) $(CXXFLAGS) -o fluidanimate_vis -DENABLE_VISUALIZATION serial.cpp cellpool.cpp fluidview.cpp -lGL -lGLU -lglut

clean:
	$(RM) *.o fluidanimate fluidanimate_vis
