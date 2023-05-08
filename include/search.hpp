#include "search.h"

std::tuple<std::vector<std::string>, int> Search::get_solution()
{
    return std::make_tuple(this->solution_sequence, this->min_ntrans3);
}
void Search::set_contract_labels(std::vector<std::string>& label)
{
    for (auto l : label) this->contract_label.push_back(l);
} 
void Search::set_tensors_labels(std::vector<std::vector<std::string>>& labels)
{
    for (auto l : labels) this->tensors_label.push(l);
}
void Search::clear_all()
{
    this->contract_label.clear();
    while (!this->tensors_label.empty()) this->tensors_label.pop();
    this->solution_sequence.clear();
    this->min_ntrans3 = INT16_MAX;
    this->tmp_ntrans3 = 0;
}
void Search::run_323()
{
    std::vector<std::string> X = this->tensors_label.front();
    this->opt_now = &X;
    this->tensors_label.pop();
    std::vector<std::string> Y = tensors_label.front();

    // search a solution once
    this->tmp_ntrans3 = 0;
    
    // once search 
    
    while (!tensors_label.empty())
    {
        std::cout << "tensors_label.size()=" << tensors_label.size() << std::endl;

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
    std::cout << "tensor_label_empty: " << this->tensors_label.empty() << std::endl;
    assert(this->tensors_label.empty());
    this->min_ntrans3 = tmp_ntrans3;
}

std::string Search::run_gemm_32(std::vector<std::string>& X, std::vector<std::string>& Y, bool ex)
{
    //test
    std::cout << "X: " << X[0] << " " << X[1] << " " << X[2] << std::endl;
    std::cout << "Y: " << Y[0] << " " << Y[1] << std::endl;
    
    std::stringstream ss;
    if (ex)
    {   //only influence trans_in_gemm 
        ss << Opts::swp;
    }
    /// caution: in which step  set label_mul???
    //1. trans3
    // std::cout << "X before trans3: " << X[0] << X[1] << X[2] << std::endl;
    // std::cout << "Y before trans3: " << Y[0] << Y[1] << std::endl;
    std::string tmp_trans = deduce.if_trans_32(X, Y);
    if (tmp_trans == Opts::t3)
    {
        *opt_now = Opts::label3_trans(*opt_now);
        ++tmp_ntrans3;
    }
    if(tmp_ntrans3 >= this->min_ntrans3) return Opts::failed;    // cut 1
    ss << tmp_trans;

        std::cout<<"1. trans3: "<<tmp_trans<<std::endl;

    //2. merge 3
    std::string tmp_merge = deduce.which_merge_32(X, Y);
    if (tmp_merge == Opts::failed) return Opts::failed;    // cut 2
    *opt_now = Opts::label3_merge(*opt_now, tmp_merge);
    ss << tmp_merge;

    std::cout<<"2. merge: "<<tmp_merge<<std::endl;

    
    //3. trans_in_gemm,  contract and split
    std::string tmp_gemm = ex ? deduce.trans_in_gemm(Y, X) : deduce.trans_in_gemm(X, Y);
    if (tmp_gemm == Opts::failed) return Opts::failed;    // cut 3
    ss << tmp_gemm;
    
    //test 
    std::cout << ss.str() << std::endl;
    //end test
    *opt_now = ex ? Opts::label22_contract(Y, X, tmp_gemm) : Opts::label22_contract(X, Y, tmp_gemm);
    std::cout << "X after label22: " << X[0] << X[1] << std::endl;
    std::cout << "Y after label22: " << Y[0] << Y[1] << std::endl;
    *opt_now = Opts::label2_split(*opt_now);

    this->tensors_label.pop();
    // remove contracted label from contract_list
    this->contract_label.remove(deduce.get_label_mul());
    X = *opt_now;
    Y = this->tensors_label.front();

    return ss.str();
}


std::string Search::run_gemm_33(std::vector<std::string>& X, std::vector<std::string>& Y)
{
    //test
    std::cout << "X: " << X[0] << X[1] << X[2] << std::endl;
    std::cout << "Y: " << Y[0] << Y[1] << Y[2] << std::endl;
    assert(opt_now == &X);
    std::stringstream ss;
    
    // 1. trans and merge
    std::string tmp_trans3_merge = deduce.how_trans_merge_33(X, Y);
    if (tmp_trans3_merge == Opts::failed) return Opts::failed;    // cut 1
    // count ntrans
    this->tmp_ntrans3 += tools::count_char(tmp_trans3_merge, Opts::t3[0]);
    if (this->tmp_ntrans3 >= this->min_ntrans3) return Opts::failed;    // cut 2
    
    Opts::label33_trans_merge(X, Y, opt_now, tmp_trans3_merge);

    //2. trans_in_gemm, contract
    std::string tmp_gemm = deduce.trans_in_gemm(X, Y);
    if (tmp_gemm == Opts::failed) return Opts::failed;    // cut 3
    ss << tmp_trans3_merge << tmp_gemm;
    Opts::label22_contract(X, Y, tmp_gemm);
    this->tensors_label.pop();
    std::cout << "tensor_label_empty: " << this->tensors_label.empty() << std::endl;
    //std::cout << "tensros_label_size(after run_gemm_33): " << tensors_label.size() << std::endl;

    //test
    std::cout << "finally out label: " << X[0] << X[1] << std::endl;
    
    return ss.str();
}
