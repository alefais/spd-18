//------------------------------------------------------------------------------
//
// Kernel:  vadd
//
// Purpose: Compute the element-wise sums
//
//          c = a + b; d = c + e; f = d + g
// 
// Input:
//          a, b, e and g float vectors of length count
//
// Output:
//          c float vector of length count holding the sum a + b
//          d float vector of length count holding the sum a + b + e
//          f float vector of length count holding the sum a + b + e + g
//
// Student: Alessandra Fais - SPD course @ UniPi
//
//------------------------------------------------------------------------------

__kernel void vadd(                             
   __global float* a,                      
   __global float* b,                      
   __global float* c,
   __global float* d,
   __global float* e,
   __global float* f,
   __global float* g,
   const unsigned int count)               
{                                          
   int i = get_global_id(0);               
   if(i < count)  {
       c[i] = a[i] + b[i];
       d[i] = c[i] + e[i];
       f[i] = d[i] + g[i];
   }
}