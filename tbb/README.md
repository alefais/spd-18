## 01 - SAXPY
Implementation of the scaled vector addition of single-precision floating point data operation.

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
Implementation of the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set), i.e. the set of all points ```c``` in the complex plane that do not go to infinity when the quadratic function ```z ← z^2 + c``` is iterated. <ul><li><b>Version 1:</b> the frames containing the representation of the Mandelbrot set have been drawn by using the [SDL2](https://wiki.libsdl.org/) library.
#### Requirements
Both the Intel TBB and SDL2 libraries are required.
#### Compile and run
A Makefile has been provided. The application can be run as

```./main par_deg```

where ```par_deg = 0``` means sequential execution.</li><li><b>Version 2:</b> a single image of the Mandelbrot set is produced as [.ppm](http://paulbourke.net/dataformats/ppm/) file.
#### Requirements
The Intel TBB library is required.
#### Compile and run
A Makefile has been provided. The application can be run as

```./main par_deg```

where ```par_deg = 0``` means sequential execution and ```par_deg > 0``` means parallel execution.</li></ul>