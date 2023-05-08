#include "deduce.hpp"
#include "tools.hpp"
#include <queue>
#include <tuple>
#include <list>
#include <sstream>

// for test
#include <iostream>
class Search
{
public:
    Search() {};
    ~Search() {};
    void set_contract_labels(std::vector<std::string>& label) { this->contract_label = label; }
    
    std::tuple<std::vector<std::string>, int> get_solution()
    {
        return std::make_tuple(this->solution_sequence, this->min_ntrans3);
    }
    void set_tensors_labels(std::vector<std::vector<std::string>>& labels)
    {
        for (auto l : labels) this->tensors_label.push(l);
    }
    void run_323()
    {
        std::vector<std::string>& X = this->tensors_label.front();
        this->opt_now = &X;
        this->tensors_label.pop();
        std::vector<std::string>& Y = tensors_label.front();

        while (!tensors_label.empty())
        {
            // search a solution
            this->tmp_ntrans3 = 0;
            std::stringstream ss;
            
            if (Y.size() == 2)   // for each gemm32
            {
                //travesrse all the  degree of freedom
                for (bool ex : this->exchange)
                {
                    ss << this->run_gemm_32(X, Y, ex);
                    if (ss.str() == Opts::failed) continue;   //try the next exchange
                }
                if (ss.str() == Opts::failed)
                    throw std::invalid_argument("Search failed. Invalid cal-label or tensors-label.");
            }
            else if (Y.size() == 3) // finaly, gemm33
            {
                // now there must remains 2 labels in contract_label
                assert(this->contract_label.size() == 2);
                //travesrse all the  degree of freedom
                for (std::string label_mul : {contract_label.front()+ contract_label.back(), contract_label.back() + contract_label.front()})
                {
                    deduce.set_label_mul(label_mul);
                    ss << this->run_gemm_33(X, Y);
                    if (ss.str() == Opts::failed) continue;   //try the next label_mul
                }
                if (ss.str() == Opts::failed)
                    throw std::invalid_argument("Search failed. Invalid cal-label or tensors-label.");
            }
            else
                throw std::invalid_argument("Invalid tensors-label.");
            
            //here means succeed, update the solution
            this->solution_sequence.push_back(ss.str());
            this->min_ntrans3 = tmp_ntrans3;
        }
    }



private:
    std::list<std::string> contract_label;    //empty means success
    std::queue<std::vector<std::string>> tensors_label;
    std::vector<std::string>* opt_now=nullptr;

    //solution 
    int min_ntrans3 = INT16_MAX;
    int tmp_ntrans3 = 0;
    std::vector<std::string> solution_sequence;

    //opts to traverse
    std::vector<bool> exchange = { false, true };
    Deduce deduce;

    std::string run_gemm_32(std::vector<std::string>& X, std::vector<std::string>& Y, bool& ex)
    {
        std::stringstream ss;
        if (ex)
        {   //only influence trans_in_gemm 
            ss << Opts::swp;
        }
        /// caution: in which step  set label_mul???
        //1. trans3
        std::string tmp_trans = deduce.if_trans_32(X, Y);
        if (tmp_trans == Opts::t3)
        {
            *opt_now = Opts::label3_trans(*opt_now);
            ++tmp_ntrans3;
        }
        if(tmp_ntrans3 >= this->min_ntrans3) return Opts::failed;    // cut 1
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
        //test 
        std::cout << ss.str() << std::endl;
        //end test
        *opt_now = ex ? Opts::label22_contract(X, Y, tmp_gemm) : Opts::label22_contract(Y, X, tmp_gemm);
        *opt_now = Opts::label2_split(*opt_now);
        this->tensors_label.pop();
        // remove contracted label from contract_list
        this->contract_label.remove(deduce.get_label_mul());
        X = *opt_now;
        Y = this->tensors_label.front();

        return ss.str();
    }


    std::string run_gemm_33(std::vector<std::string>& X, std::vector<std::string>& Y)
    {
        assert(opt_now == &X);
        std::stringstream ss;
        // 1. trans and merge
        std::string tmp_trans3_merge = deduce.how_trans_merge_33(X, Y);
        if (tmp_trans3_merge == Opts::failed) return Opts::failed;    // cut 1
        // count ntrans
        this->tmp_ntrans3 += tools::count_char(tmp_trans3_merge, Opts::t3[0]);
        if (this->tmp_ntrans3 >= this->min_ntrans3) return Opts::failed;    // cut 2

        // to do : change this to a subfuntion in Opts: apply_trans_merge_33
        for (const char s : tmp_trans3_merge)
        {
            if (s == Opts::t3[0]) *opt_now = Opts::label3_trans(*opt_now);
            else if (s == Opts::m01[0]) *opt_now = Opts::label3_merge(*opt_now, Opts::m01);
            else if (s == Opts::m12[0]) *opt_now = Opts::label3_merge(*opt_now, Opts::m12);
            else if (s == Opts::to_right[0]) opt_now = &Y;
            std::cout << "s=" <<s<< std::endl;
        }

        //2. trans_in_gemm, contract
        std::string tmp_gemm = deduce.trans_in_gemm(X, Y);
        if (tmp_gemm == Opts::failed) return Opts::failed;    // cut 3
        ss << tmp_gemm;
        Opts::label22_contract(X, Y, tmp_gemm);

        //test
        std::cout << "finally out label: " << X[0] << X[1] << std::endl;
        
        return ss.str();
    }

};