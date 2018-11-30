//------------------------------------------------------------------------------
//
// Kernel:  vadd
//
// Purpose: Compute the element-wise sum
//
//          d = a + b + c
//
// Input:
//          a, b and c float vectors of length count
//
// Output:
//          d float vector of length count holding the sum a + b + c
//
// Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

__kernel void vadd(                             
   __global float* a,                      
   __global float* b,                      
   __global float* c,
   __global float* d,
   const unsigned int count)               
{                                          
   int i = get_global_id(0);               
   if(i < count)  {
       d[i] = a[i] + b[i] + c[i];
   }
}                                          