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
Implementation of matrix multiplication between two constant matrices. Each work-item computes a single element of the matrix ```C``` and uses *global memory* only.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./mult``` without paramenters.

<br>

## 04 - Optimized matrix multiplication (use of private memory)
Optimized version of the previous exercise where *private memory* is used to minimize memory movement costs improving the performance of the program. At first only the *global memory* has been used, either if each work-item computes a single element of ```C``` or if it computes a full row of ```C```. A first optimization of the *kernel* (the version in which a work-item computes one row of ```C```) exploits the properties of locality and reuse on a single row of A: for each element ```C[i][j]``` the whole row ```A[i]``` is used, so the work-item can copy it into its own *private memory* avoiding the overhead of pulling it from *global memory* for each ```C[i][j]``` computation. A second optimization comes from storing the columns of ```B``` into the *local memory* of the work-group since all the work-items of a work-group access the same columns of ```B``` (implemented in the next exercise).

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./mult kernel_type``` where ```kernel_type``` can be 
* ```0``` : execute kernel where each work-item computes a single element of ```C```, *global memory* only is used
* ```1``` : execute kernel where each work-item computes a full row of ```C```, *global memory* only is used
* ```2``` : execute kernel where each work-item computes a full row of ```C```, *global memory* is used to access ```B``` columns, *private memory* is used to access ```A``` row

<br>

## 05 - Optimized matrix multiplication (use of local memory)
Optimized version of the matrix multiplication program where *local memory* has been used to minimize memory movement costs and optimize performance. The *kernel* is optimized in such a way that each work-item computes a full row of ```C```, *local memory* is used to access ```B``` columns and *private memory* is used to access ```A``` row.

#### Requirements
The OpenCL framework is required.

#### Compile and run
A Makefile has been provided. The application can be run as ```./mult comp_type```, where the parameter ```comp_type``` can assume value
* ```0``` : sequential computation
* ```1``` : one element per work-item
* ```2``` : one row per work-item
* ```3``` : one row per work-item, ```A``` row in *private memory*
* ```4``` : one row per work-item, ```A``` row in *private memory*, ```B``` columns in *local memory*

<br>

<!--
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

-->
