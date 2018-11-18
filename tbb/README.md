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
Implementation of the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set), i.e. the set of all points ```c``` in the complex plane that do not go to infinity when the quadratic function ```z ← z^2 + c``` is iterated.

### Version 1
The [SDL2](https://wiki.libsdl.org/) library is used draw the Mandelbrot set.

#### Requirements
Both the Intel TBB library and SDL2 library are required.

#### Compile and run
A Makefile has been provided. The application can be run as

```./main par_deg```

where ```par_deg = 0``` means sequential execution.

### Version 2
The Mandelbrot set image is produced as [.ppm](http://paulbourke.net/dataformats/ppm/) file.

#### Requirements
The Intel TBB library is required.

#### Compile and run
A Makefile has been provided. The application can be run as

```./main par_deg```

where ```par_deg = 0``` means sequential execution and ```par_deg > 0``` means parallel execution.