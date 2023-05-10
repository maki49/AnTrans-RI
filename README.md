# AnTrans-RI

This code is a Tensor-Transpose Analyzer for  [LibRI](https://github.com/deepmodeling/LibRI). 

( also, it represents my Entrance into the underlying algorithm of [LibRI](https://github.com/deepmodeling/LibRI). )

Given the labels of a list of  2-or-3-dimentional tensors to be multiplied, this code will search an optimized sequence of following operations:
> - `tensor3_transpose`
> - `tensor3_merge`
> - `gemm('NN'/'TN'/'NT'/'NN')`
> - exchange

to do the multiplication correctly, which minimizes the number of calls to the time-consuming function`tensor3_transpose`.

---

## Usage

make with googletest and run `./search_test > result.txt`. 

---

## Results

`cal_func_b01`s in [LibRI](https://github.com/deepmodeling/LibRI) have been written as test cases. 

`cal_func_bx2`s in [LibRI](https://github.com/deepmodeling/LibRI) are input to the search algorithm. 

- Check if the [results](https://github.com/maki49/AnTrans-RI/blob/master/result.txt) are correct ?  [@PeizeLin](https://github.com/PeizeLin)

---

Actually, I don't think this code is really useful, you know ...

I just regard it as a game, which reminds me of my undergraduate years.