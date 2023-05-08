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
    
    std::tuple<std::vector<std::string>, int> get_solution();
    void set_contract_labels(std::vector<std::string>& label);
    void set_tensors_labels(std::vector<std::vector<std::string>>& labels);
    void clear_all();
    
    void run_323();

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

    std::string run_gemm_32(std::vector<std::string>& X, std::vector<std::string>& Y, bool ex);

    std::string run_gemm_33(std::vector<std::string>& X, std::vector<std::string>& Y);

};