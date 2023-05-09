#include "search.h"
#include <stack>
class Search_Backtrace : public Search
{
public:
    Search_Backtrace() {};
    ~Search_Backtrace() {};
    void run_323_backtrace();
    void clear_all()
    {
        Search::clear_all();
        while (!this->previous_tersor3.empty()) this->previous_tersor3.pop();
        while (!this->origin_tersor2.empty()) this->origin_tersor2.pop();
        while (!this->previous_labelmul.empty()) this->previous_labelmul.pop();
        this->tmp_solution.clear();
    }

private:
    std::vector<std::string> tmp_solution;
    std::stack<std::vector<std::string>> previous_tersor3;
    std::stack<std::vector<std::string>> origin_tersor2;
    std::stack<std::string> previous_labelmul;
    void record_solution();
    void recover_top(bool failed);
    void stash(std::vector<std::string> X, std::vector<std::string> Y, std::string label_mul);

};
void Search_Backtrace::run_323_backtrace()
{
    for (bool exchange : {false, true})
    {
        std::vector<std::string> X = this->tensors_label.top();
        this->opt_now = &X;
        this->tensors_label.pop();
        std::vector<std::string> Y = this->tensors_label.top();

        if (Y.size() == 2)
        {
            // exchange or not is the only degree of freedom to traverse

                std::cout<<"into 32. exchange= "<<exchange<<std::endl;
                std::string label_now = tools::find_common_32(X, Y);
                deduce.set_label_mul(label_now);
                this->stash(X, Y, label_now);
                std::string s32 = this->run_gemm_32(X, Y, exchange);
                if (s32 == Opts::failed)
                {
                    recover_top(true);
                    continue;
                }
                else
                {   //X and Y have been updated in run_gemm_32
                    std::cout<<"s32="<<s32<<std::endl;
                    this->tmp_solution.push_back(s32);
                    this->tensors_label.push(X);    //intermediate result
                    this->run_323_backtrace();  //recursion
                }
        }
        else if (Y.size() == 3)
        {
            std::cout << "into 33. tmp_solution:" << std::endl;
            for (auto s : this->tmp_solution) std::cout << s << std::endl;
            assert(this->contract_label.size() == 2);
            std::string s33;
            for (std::string label_mul : {contract_label.front() + contract_label.back(), contract_label.back() + contract_label.front()})
            {
                deduce.set_label_mul(label_mul);
                std::cout<<"label_mul="<<label_mul<<std::endl;
                s33 = this->run_gemm_33(X, Y);
                std::cout << "s33=" << s33 << std::endl;
                if (s33 == Opts::failed) continue;   //try another order of label_mul
                else
                {
                    this->tmp_solution.push_back(s33);
                    //assert succeed and record the solution
                    assert(this->tmp_solution.size() == this->ngemm && this->tmp_ntrans3 < this->min_ntrans3);
                    this->record_solution();
                    this->recover_top(false);
                    break;
                }
            }
            if (s33 == Opts::failed)
            {
                std::cout << "previous_tersor3.top=" << previous_tersor3.top()[0] <<  previous_tersor3.top()[1]<<previous_tersor3.top()[2]<< std::endl;
                std::cout << "origin_tersor2.top=" << origin_tersor2.top()[0]<<origin_tersor2.top()[1] << std::endl;
                recover_top(true);
                continue;
            }
        }
        else throw std::invalid_argument("Y.size() is not 2 or 3");
    }
}


void Search_Backtrace::record_solution()
{
    this->min_ntrans3 = this->tmp_ntrans3;
    this->solution_sequence = this->tmp_solution;
}

void Search_Backtrace::recover_top(bool failed)
{
    // recover Y,  labelmul and solution when succeed
    if (!failed)
    {
        std::cout << "recovering after succeed" << std::endl;
        for (size_t i : {0, 1})   //pop twice
        {
            this->tmp_ntrans3 -= tools::count_char(this->tmp_solution.back(), Opts::t3[0]);
            this->tmp_solution.pop_back();
        }
        assert(this->origin_tersor2.top().size() == 2); //Y.size
        this->tensors_label.push(this->origin_tersor2.top());
        //recover label_mul
        this->contract_label.push_front(this->previous_labelmul.top());
    }
    this->origin_tersor2.pop();
    this->previous_labelmul.pop();
    //recover X
    this->tensors_label.push(this->previous_tersor3.top());
    this->previous_tersor3.pop();

    assert(this->tensors_label.size() == 3);
    
}
void Search_Backtrace::stash(std::vector<std::string> X,  std::vector<std::string> Y, std::string label_mul)
{
    assert(X.size() == 3);
    assert(Y.size() == 2);
    this->previous_tersor3.push(X);
    this->origin_tersor2.push(Y);
    this->previous_labelmul.push(label_mul);
}
