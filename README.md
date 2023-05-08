# AnTrans-RI

This code is a Tensor-Transpose Analyzer for  [LibRI](https://github.com/deepmodeling/LibRI). 

( also, it represents my Entrance into the underlying algorithm of [LibRI](https://github.com/deepmodeling/LibRI). )

Given the labels of a list of  2-or-3-dimentional tensors to be multiplied, this code will search an optimized sequence of following operations:
> - `tensor3_transpose`
> - `tensor3_merge`
> - `gemm('NN'/'TN'/'NT'/'NN')`
> - exchange

to do the multiplication correctly, which minimizes the number of calls to the time-consuming function`tensor3_transpose`.
