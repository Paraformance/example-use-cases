## Introduction 

The FluidAnimate application is taken from the
PARSEC benchmark suite.  See
the [main PARSEC site](http://parsec.cs.princeton.edu/index.htm) and also the 
[PARSEC wiki](http://wiki.cs.princeton.edu/index.php/PARSEC).

The application simulates a fluid sloshing about inside a box with a
square cross-section; the input consists of a file containing the
initial positions and velocities of a number of particles, and the
program simulates the evolution of the system over a number of
time-steps, optionally writing the final positions and velocities of
the particles to a file.  There are two versions of the program: one
of these provides an animated visualisation of the evolution of the
system until the user terminates it; the other just performs the
calculations for a number of time-steps specified on the command line, and
is more suitable for benchmarking because the overhead of producing
the graphics is avoided.

Two input files are provided here: `inputs/in_5K.fluid` and
`inputs/in_15K.fluid`, containing about 5000 and 15000 particles
respectively.  More inputs (up to 500000 particles) can be found 
[here](http://akaros.cs.berkeley.edu/files/parsec-data/fluidanimate/);
these are also distributed with parsec-3.0.

The PARSEC distribution contains a serial version of the benchmark
together with hand-written pthreads and TBB implementations; these are
not included here.  

Note that when we parallelise the program we should expect race
conditions.  The reason is that each cell in simulation space is
updated depending on the states of 26 neighbouring cells (away from
the boundaries of the simulation, at least), and if the program is
running in parallel then the order of the updates may vary.  See [this
post](https://lists.cs.princeton.edu/pipermail/parsec-users/2012-October/001446.html)
on the parsec-users mailing list, where this is described as **expected
behaviour** of the application.

Note that this version of the code has been extended slightly to
report the simulation time (excluding data read/write times) in the
non-visualisation version.  This uses functions from `std::chrono`, so
the application must be compiled with at least C++11 (`-std=c++11` or
`-std=c++0x`).

### Dependences
1. OpenGL
2. GLUT
