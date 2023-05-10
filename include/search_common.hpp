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
    this->ngemm=labels.size()-1;
    for (auto iter = labels.rbegin(); iter != labels.rend(); ++iter)
        this->tensors_label.push(*iter);
}
void Search::clear_all()
{
    this->contract_label.clear();
    while (!this->tensors_label.empty()) this->tensors_label.pop();
    this->solution_sequence.clear();
    this->min_ntrans3 = INT16_MAX;
    this->tmp_ntrans3 = 0;
}

std::string Search::run_gemm_32(std::vector<std::string>& X, std::vector<std::string>& Y, bool ex)
{
    //test
    // std::cout << "X: " << X[0] << " " << X[1] << " " << X[2] << std::endl;
    // std::cout << "Y: " << Y[0] << " " << Y[1] << std::endl;
    
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
    if (tmp_ntrans3 >= this->min_ntrans3)
    {
        --tmp_ntrans3;  //minus back
        return Opts::failed;    // cut 1
    }

    ss << tmp_trans;

    //2. merge 3
    std::string tmp_merge = deduce.which_merge_32(X, Y);
    if (tmp_merge == Opts::failed) return Opts::failed;    // cut 2
    *opt_now = Opts::label3_merge(*opt_now, tmp_merge);
    ss << tmp_merge;
    
    //3. trans_in_gemm,  contract and split
    std::string tmp_gemm = ex ? deduce.trans_in_gemm(Y, X) : deduce.trans_in_gemm(X, Y);
    if (tmp_gemm == Opts::failed) return Opts::failed;    // cut 3
    ss << tmp_gemm;

    // if not failed, do the contract and update X and Y 
    *opt_now = ex ? Opts::label22_contract(Y, X, tmp_gemm) : Opts::label22_contract(X, Y, tmp_gemm);

    *opt_now = Opts::label2_split(*opt_now);
    //update X and Y
    this->tensors_label.pop();
    X = *opt_now;
    Y = this->tensors_label.top();
    // remove contracted label from contract_list
    // std::cout<<"removing label_mul" << deduce.get_label_mul() << std::endl;
    this->contract_label.remove(deduce.get_label_mul());
    // std::cout << "X after label22gemm: " << X[0] << X[1] <<X[2]<< std::endl;
    // std::cout << "Y after label22gemm: " << Y[0] << Y[1] << std::endl;
    return ss.str();
}


std::string Search::run_gemm_33(std::vector<std::string>& X, std::vector<std::string>& Y)
{
    //test
    // std::cout << "X: " << X[0] << X[1] << X[2] << std::endl;
    // std::cout << "Y: " << Y[0] << Y[1] << Y[2] << std::endl;
    assert(opt_now == &X);
    std::stringstream ss;

    // 1. trans and merge
    std::string tmp_trans3_merge = deduce.how_trans_merge_33(X, Y);
    if (tmp_trans3_merge == Opts::failed) return Opts::failed;    // cut 1
    // count ntrans
    size_t count_t3 = tools::count_char(tmp_trans3_merge, Opts::t3[0]);
    this->tmp_ntrans3 += count_t3;
    if (this->tmp_ntrans3 >= this->min_ntrans3)
    {
        this->tmp_ntrans3 -= count_t3;  //minus back
        return Opts::failed;    // cut 2
    }

    Opts::label33_trans_merge(X, Y, opt_now, tmp_trans3_merge);

    //2. trans_in_gemm, contract
    std::string tmp_gemm = deduce.trans_in_gemm(X, Y);
    if (tmp_gemm == Opts::failed) return Opts::failed;    // cut 3
    ss << tmp_trans3_merge << tmp_gemm;
    //Opts::label22_contract(X, Y, tmp_gemm);
    
    return ss.str();
}
