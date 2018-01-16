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

You need to install the glut library (freeglut3 and
freeglut3-dev on Ubuntu) for visualisations. `sudo apt-get install
freeglut3 freeglut3-dev` does the trick on Ubuntu. Something different
may be required on OSX, and I have no idea what's required on Windows
(please edit this file if you find out).

### Compiling and running from the command line 

A simple makefile is
included which allows you to compile the program from the command
line: just type `make` and you should get two executable file:
`fluidanimate` (no visualisation) and `fluidanimate_vis` (including
visualisation).

To run the program, type a command like 

```
./fluidanimate 1 100 inputs/in_5K.fluid'
```

This will run the program for 100 timesteps.  If you wish to save the final state
of the simulation, add a filename at the end of the command line; this is probably
not very useful for our purposes.

The first argument represents the number of threads in PARSEC's parallel versions
of the program (not included here) and must always be 1.

If you run into problems at this point, it could be due to problems
with the graphics environment, for example with the driver for your
graphics card.  In this case, look at the error messages and seek help
from Google.

If you perform this step, you should type `make clean` once you've finished to clean
up the directory and remove some files which we don't want to import into Eclipse.

### Importing the source code into Eclipse

This assumes that you want to create a project that will keep separate copies of the
source files. This is convenient for modifying the files freely without having to
worry about overwriting stuff. If, however, you want the project to directly manage
files in the repo, in step 1, when choosing a location of the source files,
instead of using a default location (`$HOME/workspace` or similar), you need to choose
the fluidanimate directory from the Git repository. This is useful if you want to
make changes to the code that will have to be committed back to the repository,
as  Eclipse recognises that the directory is a part of the Git repository and
gives you a nice set of tools for committing/pushing changes. In this case, you can
omit step 2 below.

When we create the project, we'll create it in such a way that Eclipse manages the build process for us using objects called "build configurations". 
The process which follows is rather complicated because we will create
two build configurations: one including visualisation (for
showing to people), and one without (for demonstrating speedups).  We will
also need to create two run configurations, for running the two different executables
produced by the different build configurations.


1. Go to `File -> New -> C++ Project`
  * Project type: Empty Project
  *  Toolchains: Linux GCC
  *  Name: FluidAnimate
  *  Click on `Next` and you should see a dialogue saying "Select Configurations".  Untick "Debug"; this will associate a build configuration called "Release" with the project, and later on we'll modify that and tell it how to build the project.  Deselecting "Debug" gets rid of a configuration which includes debugging information in the build process.
  *  Click Finish

2. We now need to set the C++ dialect which the project will use.
  * Right click on the FluidAnimate project.
  * Select `Properties` (at the very bottom of the context menu).
  * Set the C++ dialect.
      * Go to `C/C++ Build -> Settings -> GCC C++ Compiler -> Tool Settings -> Dialect` and select `ISO C++11`
      * Still in `Tool Settings`, select `Miscellaneous` and add `-std=c++0x` or `-std=c++11` at the end of `Other flags`.
  * Click on `Apply` then `OK`.
  * This process seems to fail sporadically.  This is a longstanding problem, and I still don't know how to fix it.  In this case, the symptom will be that after you build the project there will be several compilation errors involving `std::chrono` at the bottom of `serial.cpp`.  **If all else fails**, comment out the relevant lines; the effect will be that the program will no longer print out how long it took to perform the simulation.


3. Import the FluidAnimate source code.
  * Right click on the FluidAnimate project.
  * Select `Import`
  * Select `General -> File System`
  * Choose the `use-cases/FluidAnimate` directory from the `scenarios` repository. 
  * Make sure that `Create a top-level folder` is *not* ticked.  If it is, everything will be imported into a subfolder of the main project folder; this is harmless but annoying.
  * Select all files and click finish (you can deselect `Makefile` if you want, since Eclipse doesn't need it).
  * At the right hand side of the main Eclipse window, you should see the `FluidAnimate` project containing the relevant source files.

### Building and running the project

We will use two different build configurations. We will create a new build for
visualisation. This is good for demonstrating the application to people,
as we get a nice visualisation of movement of particles in the fluid.
In the default build, however, we will not use visualisation, since
the visualisation goes on continuously and is not convenient for discovering
hotspots and checking how good the parallelisation is in terms of delivered
speedups. 

4. Right click on FluidAnimate and select `Build Configurations -> Manage`.  You may have to enlarge the window a bit to see what's going on.
 * Alternatively, select `Project -> Build Configurations` in the main Eclipse menu.
 * The only configuration should be `Release`.  Make sure that's selected and click on `Rename`; change the name to `Visualisation` and click on `OK`.
 * Now click on `New...` and enter the name `NoVisualisation`: it should be copying the settings from `Visualisation`; after that click on `OK`
 * Check that `Visualisation` is the active configuration and click on `OK`.

5. Now we have to modify the `Visualisation` configuration to tell it how to build the project.
  * Right click on the FluidAnimate project
  * At the left-hand side of the `Properties` dialogue, go to `C++ General -> Paths and Symbols`. 
  * Go to `Symbols`, and click on `GNU C++`
  * Click `Add`. 
  * Under name, type `ENABLE_VISUALIZATION`, being careful to spell it properly. Leave the value empty and click `OK`. 
  * Back at `Paths and Symbols`, click on `Libraries`. Add the following libraries by clicking `Add`: `glut`, `GL`and `GLU`. 
  * You should also add `tbb` if you're planning to parallelise the application.
  * DO NOT click on 'Add to all configurations'. 
  * Click Apply, then OK.  If it asks you about rebuilding the index, say `Yes`.
  * [You can also add the `ENABLE_VISUALIZATION` symbol and the libraries in appropriate places in `Tool Settings`; this seems to have the same effect.]

6. Try building the project.
  * Right clicking on `FluidAnimate` and click `Build Project`.
  * If it succeeds, a directory called `Visualisation` should appear containing an executable file and some other things.

7. Create a new Run configuration. 
  * Right click `FluidAnimate`, then select `Run as -> Run Configurations` (you can also do this via `Run -> Run Configurations` in the main Eclipse menu).
  * Select `C/C++ Application` and then click on the `New` icon (in the top left corner). 
  * Make sure the executable is `Visualisation/FluidAnimate`. Click on `Arguments` and click inside the `Program Arguments` part.
  * Type the following (with linebreaks):

```
     1
     100
     inputs/in_15K.fluid
```
  * The execution time is proportional to the number of time-steps (the second argument), so it may be useful to alter that.
  * You can also enter the full path to the input file if you don't want to use the ones imported with the rest of the project.
  * If you change the input file (and hence the number of particles in the simulation), the execution time may change significantly.
  * You can add a fourth argument to specify an output file to save the final state of the fluid.

8. Click `Apply`, then `Run`.  This should open a GL window with a
visualisation of a moving fluid.  To terminate the application,
press the `Esc` key or click on the square red stop button in the
Eclipse console.

9. Now modify the `NoVisualisation` configuration. We need to remove
`fluidview.cpp` and `fluidview.hpp` from that build. 
  * Right click on the `fluidview.cpp` file under `FluidAnimate`, click `Resource
Configuration -> Exclude from Build`, and tick `NoVisualisation`.  
  * This will modify the `NoVisualisation` configuration so that it builds the application without any
  visualisation.
  * Note that the `NoVisualisation` build configuration will not contain the preprocessor symbol and 
  libraries added to the `Visualisation` configuration in step 5.

10. Try building the project again. 
  * Set the active build configuration to `NoVisualisation`: right click on `FluidAnimate`, select `Build Configurations -> Set Active -> NoVisualisation`.  You can also do this from the main Eclipse menu under `Project -> Build Configurations`
  * If you're going to parallelise the application, add the `tbb` library as in step 5.
  * Build the project (right click on `FluidAnimate`, then select `Build Project`).

11. Create another run configuration. 
  * The process is essentially the same as in step 6, but the executable should be `NoVisualisation/FluidAnimate` instead of `Visualisation/FluidAnimate` (use `Search Project` to find this if it's not set in the new configuration).  
  * Remember to add the program arguments.  
  * Running the new configuration will perform the same calculations as before
but with no graphics; this is more suitable for measuring the effect
of parallelisation.  The time required for the simulation should be
printed in the Eclipse console.
  * You can now run the project and select run configurations using the ![Run button](http://eclipse-icons.i24.cc/ovr16/lrun_obj.gif) button in the toolbar at the top of the Eclipse window.

### Parallelising the FluidAnimate application
To follow...