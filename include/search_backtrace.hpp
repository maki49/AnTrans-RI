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
        while (!this->origin_tensor2.empty()) this->origin_tensor2.pop();
        while (!this->previous_labelmul.empty()) this->previous_labelmul.pop();
        this->tmp_solution.clear();
    }
private:
    std::vector<std::string> tmp_solution;
    std::stack<std::vector<std::string>> previous_tersor3;
    std::stack<std::vector<std::string>> origin_tensor2;
    std::stack<std::string> previous_labelmul;
    std::tuple<std::vector<std::string>, std::vector<std::string>> init_xy();
    void record_solution();
    void step_back(bool end);
    void stash(std::vector<std::string> X, std::vector<std::string> Y, std::string label_mul);
    void solution_backspace();
};

void Search_Backtrace::run_323_backtrace()
{
    if(tensors_label.size() ==2)
    {
        // init X, Y
        auto [X, Y] = this->init_xy();
        this->opt_now = &X;
        
        std::cout << "into 33. tmp_solution:" << std::endl;
        for (auto s : this->tmp_solution) std::cout << s << std::endl;
        assert(this->contract_label.size() == 2);
        std::string s33;
        // try two orders of label_mul
        for (std::string label_mul : {contract_label.front() + contract_label.back(), contract_label.back() + contract_label.front()})
        {
            deduce.set_label_mul(label_mul);
            std::cout<<"label_mul="<<label_mul<<std::endl;
            s33 = this->run_gemm_33(X, Y);
            std::cout << "s33=" << s33 << std::endl;
            if (s33 == Opts::failed) continue;   //try another order

            //final success, record the solution
            this->tmp_solution.push_back(s33);
            //assert succeed and record the solution
            assert(this->tmp_solution.size() == this->ngemm && this->tmp_ntrans3 < this->min_ntrans3);
            this->record_solution();
            this->solution_backspace();
            break;  //only one order is useful
        }
        return;
    }
    
    // exchange or not is the only degree of freedom to traverse
    for (bool exchange : {false, true})
    {
        // init X, Y
        auto [X, Y] = this->init_xy();
        this->opt_now = &X;
        
        std::cout << "into 32. exchange= " << exchange << std::endl;
        //output X, Y
        for (auto s : X) std::cout << s << " ";
        std::cout << std::endl;
        for (auto s : Y) std::cout << s << " ";
        std::cout << std::endl;
        
        std::string label_now = tools::find_common_32(X, Y);
        deduce.set_label_mul(label_now);
        this->stash(X, Y, label_now);
        std::string s32 = this->run_gemm_32(X, Y, exchange);
        if (s32 == Opts::failed) continue;

        //go to the next step
        //X and Y have been updated in run_gemm_32
        std::cout << "s32=" << s32 << std::endl;
        this->tmp_solution.push_back(s32);
        this->tensors_label.push(X);    //intermediate result
        this->run_323_backtrace();  //recursion
        step_back(exchange);    //here exchange==true means loop end
    }
    
}

std::tuple<std::vector<std::string>, std::vector<std::string>> Search_Backtrace::init_xy()
{
    std::vector<std::string> X = this->tensors_label.top();
    this->tensors_label.pop();
    std::vector<std::string> Y = this->tensors_label.top();
    return std::make_tuple(X, Y);
}

void Search_Backtrace::record_solution()
{
    this->min_ntrans3 = this->tmp_ntrans3;
    this->solution_sequence = this->tmp_solution;
}

void Search_Backtrace::step_back(bool end)
{
    std::cout<<"stepping back"<<std::endl;
    this->solution_backspace();
    
    //recover label_mul
    this->contract_label.push_front(this->previous_labelmul.top());
    this->previous_labelmul.pop();

    //output X, Y
    std::cout<<"recovering Y:"<<std::endl;
    for (auto s : this->origin_tensor2.top()) std::cout << s << " ";
    std::cout << std::endl;
    std::cout<<"recovering X:"<<std::endl;
    for (auto s : this->previous_tersor3.top()) std::cout << s << " ";
     std::cout << std::endl;
    
    //recover Y
    this->tensors_label.push(this->origin_tensor2.top());
    this->origin_tensor2.pop();
    
    //if exchange == false (loop not end yet) recover X
    if(!end) this->tensors_label.push(this->previous_tersor3.top());
    this->previous_tersor3.pop();
}

void Search_Backtrace::stash(std::vector<std::string> X, std::vector<std::string> Y, std::string label_mul)
{
    assert(X.size() == 3);
    assert(Y.size() == 2);
    this->previous_tersor3.push(X);
    this->origin_tensor2.push(Y);
    this->previous_labelmul.push(label_mul);
}

void Search_Backtrace::solution_backspace()
{
    this->tmp_ntrans3 -= tools::count_char(this->tmp_solution.back(), Opts::t3[0]);
    this->tmp_solution.pop_back();
}
