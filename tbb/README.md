## 01 - SAXPY
Implementation of the *scaled vector addition of single-precision floating point data* operation.

#### Requirements
The Intel TBB library is required.

#### Compile and run
Compile with

```g++ -std=c++11 -Wall -Wextra -O3 main.cpp -ltbb -o main```

or

```icc -std=c++11 -Wall -Wextra -O3 main.cpp -ltbb -o main```

The application can be run as ```./main``` without parameters.

<br>

## 02 - Mandelbrot Set
Implementation of the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set), i.e. the set of all points c in the complex plane for which the quadratic function   
z = z<sup>2</sup> + c does not go to infinity when iterated from z = 0. A plot of the Mandelbrot set can be obtained by coloring values of c in the complex plane according to the number of steps required to reach r<sub>max</sub> = 2.<ul><li><b>Version 1:</b> sequential implementation that uses the [SDL2](https://wiki.libsdl.org/) library to draw in a separated window all the images produced by iterating the Mandelbrot set computation.
#### Requirements
The SDL2 library is required.
#### Compile and run
A Makefile has been provided. The application can be run as ```./main``` without parameters.
#### Interaction with the application
The user can terminate the process by closing the window or by pressing ```q``` on the keyboard to quit.</li><li><b>Version 2:</b> the plot of the Mandelbrot set is produced as [.ppm](http://paulbourke.net/dataformats/ppm/) file.
#### Requirements
The Intel TBB library is required.
#### Compile and run
A Makefile has been provided. The application can be run as

```./main par_deg```

where ```par_deg = 0``` means sequential execution and ```par_deg > 0``` means parallel execution.</li></ul>
