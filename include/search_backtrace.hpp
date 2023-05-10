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
    void init_xy();
    void record_solution();
    void step_back();
    void stash_label(std::string& label);
    void stashpop_label();
    void stash_xy();
    void stashpop_xy();
    void solution_backspace();

    std::vector<std::string> X;
    std::vector<std::string> Y;
};

void Search_Backtrace::run_323_backtrace()
{
    if (tensors_label.size() == 2)
    {
        // std::cout << "into 33. tmp_solution:" << std::endl;
        // for (auto s : this->tmp_solution) std::cout << s << std::endl;
        // //output constract_labels
        // std::cout << "contract_labels:" << std::endl;
        // for (auto s : this->contract_label) std::cout << s << std::endl;
        assert(this->contract_label.size() == 2);
        std::string s33;
        // try two orders of label_mul
        for (std::string label_mul : {contract_label.front() + contract_label.back(), contract_label.back() + contract_label.front()})
        {
            deduce.set_label_mul(label_mul);
            // std::cout<<"label_mul="<<label_mul<<std::endl;
            s33 = this->run_gemm_33(X, Y);
            // std::cout << "s33=" << s33 << std::endl;
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

    if (this->tmp_ntrans3 >= this->min_ntrans3)return;
    
    // exchange or not is the only degree of freedom to traverse
    for (bool exchange : {false, true})
    {
        if (this->tmp_ntrans3 >= this->min_ntrans3)return;
        
        // std::cout << "into 32. exchange= " << exchange << std::endl;
        // std::cout << "tmp_ntrans3" << tmp_ntrans3 << std::endl;
        // std::cout << "min_ntrans3" << min_ntrans3 << std::endl;

        this->init_xy();
        //output X, Y
        // std::cout << "X after init: ";
        // for (auto s : X) std::cout << s << " ";
        // std::cout << std::endl;
        // std::cout << "Y after init: ";
        // for (auto s : Y) std::cout << s << " ";
        // std::cout << std::endl;
        
        this->stash_xy();

        //output X, Y
        // std::cout << "X before 32: ";
        // for (auto s : X) std::cout << s << " ";
        // std::cout << std::endl;
        // std::cout << "Y before 32: ";
        // for (auto s : Y) std::cout << s << " ";
        // std::cout << std::endl;
        

        std::string label_now = tools::find_common_32(X, Y);
        deduce.set_label_mul(label_now);
        this->stash_label(label_now);
        
        std::string s32 = this->run_gemm_32(X, Y, exchange);
        // std::cout << "s32=" << s32 << std::endl;
        // //output X, Y
        // std::cout << "X after 32: ";
        // for (auto s : X) std::cout << s << " ";
        // std::cout << std::endl;
        // std::cout << "Y after 32: ";
        // for (auto s : Y) std::cout << s << " ";
        // std::cout << std::endl;
        // std::cout << "tensor-top after 32: ";
        // for (auto s : tensors_label.top()) std::cout << s << " ";
        // std::cout << std::endl;

        if (s32 == Opts::failed)
        {
            if(!exchange) tensors_label.push(X);    //for the next loop
            this->stashpop_xy();
            this->stashpop_label();
            continue;
        }

        //go to the next step
        //X and Y have been updated in run_gemm_32
        // std::cout << "s32=" << s32 << std::endl;
        this->tmp_solution.push_back(s32);

        ///prepare for recursion
        this->tensors_label.push(X);    //intermediate result
        
        this->run_323_backtrace();  //run_323_backtrace
        if(this->min_ntrans3 == 0) return;  //already have optimized solution
        step_back();    //here exchange==true means loop end
    }
    // std::cout << "tensor-top after loop: ";
    // for (auto s : tensors_label.top()) std::cout << s << " ";
    // std::cout << std::endl;
}

void Search_Backtrace::init_xy()
{
    this->X = this->tensors_label.top();
    this->tensors_label.pop();
    this->Y = this->tensors_label.top();
    this->opt_now = &this->X;
}

void Search_Backtrace::record_solution()
{
    // std::cout<<"record solution:"<<std::endl;
    this->min_ntrans3 = this->tmp_ntrans3;
    this->solution_sequence = this->tmp_solution;
}


void Search_Backtrace::step_back()
{
    // std::cout << "stepping back" << std::endl;
    
    //if the last gemm failed, the first label's size will be 3, then pop the first label in tensors_label 
    //else, do nothing
    if (this->tensors_label.top().size() == 3) this->tensors_label.pop();
    this->solution_backspace();
    
    //recover label_mul
    // std::cout<<"recovering label_mul:"<<this->previous_labelmul.top()<<std::endl;
    this->contract_label.push_front(this->previous_labelmul.top());
    this->previous_labelmul.pop();

    //output X, Y
    // std::cout<<"recovering Y:"<<std::endl;
    // for (auto s : this->origin_tensor2.top()) std::cout << s << " ";
    // std::cout << std::endl;


    //recover Y
    Y = this->origin_tensor2.top();
    // std::cout << "tensors_label.top() before push Y:" << std::endl;
    // for (auto s : this->tensors_label.top()) std::cout << s << " ";
    // std::cout << std::endl;
    this->tensors_label.push(Y);
    this->origin_tensor2.pop();


    // std::cout<<"recovering X:"<<std::endl;
    // for (auto s : this->previous_tersor3.top()) std::cout << s << " ";
    // std::cout << std::endl;
    //recover X
    X = this->previous_tersor3.top();
    this->tensors_label.push(X);
    this->previous_tersor3.pop();
}

void Search_Backtrace::stash_label(std::string& label_mul)
{
    // std::cout<<"stashing label:"<<label_mul<<std::endl;
    this->previous_labelmul.push(label_mul);
}
void Search_Backtrace::stashpop_label()
{
    // std::cout<<"poping label:"<<this->previous_labelmul.top()<<std::endl;
    this->previous_labelmul.pop();
}
void Search_Backtrace::stash_xy()
{
    assert(X.size() == 3);
    assert(Y.size() == 2);
    this->previous_tersor3.push(X);
    this->origin_tensor2.push(Y);
    // std::cout << "stashing XY：" << std::endl;
    // for (auto s : X) std::cout << s << " ";
    // std::cout << std::endl;
    // for (auto s : Y) std::cout << s << " ";
    // std::cout << std::endl;
}

void Search_Backtrace::stashpop_xy()
{
    // std::cout << "popinging stashed XY：" << std::endl;
    // for (auto s : previous_tersor3.top()) std::cout << s << " ";
    // std::cout << std::endl;
    // for (auto s : origin_tensor2.top()) std::cout << s << " ";
    // std::cout << std::endl;
    this->previous_tersor3.pop();
    // // need to recover Y ?????
    // this->tensors_label.push(Y);
    this->origin_tensor2.pop();
    
}

void Search_Backtrace::solution_backspace()
{
    this->tmp_ntrans3 -= tools::count_char(this->tmp_solution.back(), Opts::t3[0]);
    this->tmp_solution.pop_back();
}
