#include "search.h"
class Search_Once : public Search
{
public:
    Search_Once() {};
    ~Search_Once() {};
    void run_323_once();
};

void Search_Once::run_323_once()
{
    std::vector<std::string> X = this->tensors_label.top();
    this->opt_now = &X;
    this->tensors_label.pop();
    std::vector<std::string> Y = tensors_label.top();

    // search a solution once
    this->tmp_ntrans3 = 0;
    
    // once search 
    
    while (solution_sequence.size() < this->ngemm)
    {
        if (Y.size() == 2)   // for each gemm32
        {
            //travesrse all the  degree of freedom
            // for (bool ex : this->exchange)
            // {
                std::stringstream ss;
                deduce.set_label_mul(tools::find_common_32(X, Y));
                ss << this->run_gemm_32(X, Y, false);   //tmp
                // if (ss.str() == Opts::failed) continue;   //try the next exchange
            // }
                if (ss.str() == Opts::failed) break;
                else {
                    solution_sequence.push_back(ss.str());
                    continue;
                }

        }
        else if (Y.size() == 3) // finaly, gemm33
        {
            // now there must remains 2 labels in contract_label.
            // travesrse 2 orders of them.
            assert(this->contract_label.size() == 2);
            for (std::string label_mul : {contract_label.front() + contract_label.back(), contract_label.back() + contract_label.front()})
            {
                std::stringstream ss;
                deduce.set_label_mul(label_mul);
                ss << this->run_gemm_33(X, Y);
                if (ss.str() == Opts::failed) continue;   //try another order of label_mul
                else
                {
                    solution_sequence.push_back(ss.str());
                    break;
                }
            }
        }
        else throw std::invalid_argument("Invalid tensors_label.");
    }
    //update the solution
    this->min_ntrans3 = tmp_ntrans3;
}