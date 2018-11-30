## 01 - Chaining vector addition
Implementation of a chain of vector additions. A message to standard output is produced to verify the correct result.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./vadd``` without paramenters.

<br>

## 02 - Addition of three vectors
Implementation of the addition of three vectors. A message to standard output is produced to verify the correct result.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./vadd``` without paramenters.

<br>

## 03 - Multiplication of two matrices
Implementation of matrix multiplication between two constant matrices. The grain size of the *kernel* computation is a single element of the matrix ```C```.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./mult``` without paramenters.

<br>

## 04 - Optimized matrix multiplication
Optimized version of the previous exercise where private memory is used to minimize memory movement costs improving the performance of the program. The grain size of the *kernel* computation is a row of the matrix ```C```. Two versions of the *kernel* have been provided: one is not optimized and each work-item works on the *global memory*, in the other each work-item copies its own row of ```A`` into *private memory*.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./mult``` without paramenters.

<br>

## 05 -
Implementation of

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./vadd``` without paramenters.

<br>

## 06 -
Implementation of

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./vadd``` without paramenters.

<br>

## 07 -
Implementation of

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./vadd``` without paramenters.
