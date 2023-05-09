#include <gtest/gtest.h>
#include "../include/search_common.hpp"
#include "../include/search_once.hpp"
#include "../include/search_backtrace.hpp"
struct Case
{
    Case(std::vector<std::vector<std::string>> tensors_label,
        std::vector<std::string> contract_label,
        std::vector<std::string> output_label,
        std::vector<std::string> ref_solution_sequence,
        int ref_min_ntrans3)
        : tensors_label(tensors_label),
        contract_label(contract_label),
        output_label(output_label),
        ref_solution_sequence(ref_solution_sequence),
        ref_min_ntrans3(ref_min_ntrans3) {}
    ~Case() {}
    std::vector<std::vector<std::string>>  tensors_label;
    std::vector<std::string> contract_label;
    std::vector<std::string> output_label;
    std::vector<std::string> ref_solution_sequence;
    int ref_min_ntrans3;
};
class SearchTest : public testing::Test
{
protected:
    std::vector<Case> case_list_once{   //cal_b01
        Case(   //a0b0_a1b1
        { {"a0", "a1", "a2"}, {"a0", "b0"}, {"a1", "b1"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a1", "b1" },
        { "a2", "b2" },
        { Opts::m12 + "TN", Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::m01 + "NN" },
        0), 
        Case(   //2
        { {"a0", "a1", "a2"}, {"a1", "b0"}, {"a0", "b1"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a1", "b1" },
        { "a2", "b2" },
        { Opts::t3 + Opts::m12 + "TN", Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::m01 + "NN" },
        1),
        Case(   //3
        { {"a0", "a1", "a2"}, {"a0", "b0"}, {"a2", "b1"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a2", "b1" },
        { "a1", "b2" },
        { Opts::m12 + "TN", Opts::t3 + Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::m01 + "NN" },
        1),
        Case(   //4
        { {"a0", "a1", "a2"}, {"a2", "b0"}, {"a0", "b1"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a2", "b1" },
        { "a1", "b2" },
        { Opts::m01 + "NN", Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::m01 + "NN" },
        0),
        Case(   //5
        { {"a0", "a1", "a2"}, {"a2", "b1"}, {"a1", "b0"}, {"b0", "b1", "b2"} },
        { "a1", "b0", "a2", "b1" },
        { "a0", "b2" },
        { Opts::m01 + "NN", Opts::t3 + Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::t3 + Opts::m01 + "NN" },
        2)
        //...add more ...
    };
    
    std::vector<Case> case_list_optimized_b01{  //cal_b01
        case_list_once[0],
        case_list_once[1],
        case_list_once[2],
        case_list_once[3],
        Case(   //5
        { {"a0", "a1", "a2"}, {"a2", "b1"}, {"a1", "b0"}, {"b0", "b1", "b2"} },
        { "a1", "b0", "a2", "b1" },
        { "a0", "b2" },
        { Opts::swp + Opts::m01 + "TT", Opts::swp + Opts::m01 + "TT", Opts::m01 + Opts::to_right + Opts::m01 + "TN" },
        0),
        Case(   //6
        { {"a0", "a1", "a2"}, {"a2", "b0"}, {"a1", "b1"}, {"b0", "b1", "b2"} },
        { "a1", "b0", "a2", "b1" },
        { "a0", "b2" },
        { Opts::m01 + "NN", Opts::t3 + Opts::m12 + "TN", Opts::m12 + Opts::to_right  + Opts::m01 + "NN" },
        1)
    };

    std::vector<Case> case_list_optimized_bx2{  //cal_b01
        Case(   //1-1
        { {"a0", "a1", "a2"}, {"a0", "b0"}, {"a1", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a1", "b2" },
        { "a2", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //1-2
        { {"a0", "a1", "a2"}, {"a0", "b1"}, {"a1", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a1", "b2" },
        { "a2", "b0" },
        { "NN" },   //tmp
        0),
        Case(   //2-1
        { {"a0", "a1", "a2"}, {"a1", "b0"}, {"a0", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b2", "a1", "b0" },
        { "a2", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //2-2
        { {"a0", "a1", "a2"}, {"a1", "b1"}, {"a0", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b2", "a1", "b0" },
        { "a2", "b0" },
        { "NN" },   //tmp
        0),
        Case(   //3-1
        { {"a0", "a1", "a2"}, {"a1", "b0"}, {"a2", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a2", "b2" },
        { "a1", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //3-2
        { {"a0", "a1", "a2"}, {"a1", "b1"}, {"a2", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b0", "a2", "b2" },
        { "a1", "b0" },
        { "NN" },   //tmp
        0),
        Case(   //4-1
        { {"a0", "a1", "a2"}, {"a2", "b0"}, {"a0", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b2", "a2", "b1" },
        { "a1", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //4-2
        { {"a0", "a1", "a2"}, {"a2", "b1"}, {"a0", "b2"}, {"b0", "b1", "b2"} },
        { "a0", "b2", "a2", "b1" },
        { "a1", "b0" },
        { "NN" },   //tmp
        0),
        Case(   //5-1
        { {"a0", "a1", "a2"}, {"a1", "b0"}, {"a2", "b2"}, {"b0", "b1", "b2"} },
        { "a1", "b0", "a2", "b2" },
        { "a0", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //5-2
        { {"a0", "a1", "a2"}, {"a2", "b0"}, {"a1", "b2"}, {"b0", "b1", "b2"} },
        { "a1", "b2", "a2", "b0" },
        { "a0", "b1" },
        { "NN" },   //tmp
        0),
        Case(   //5-2
        { {"a0", "a1", "a2"}, {"a2", "b1"}, {"a1", "b2"}, {"b0", "b1", "b2"} },
        { "a1", "b2", "a2", "b0" },
        { "a0", "b0" },
        { "NN" },   //tmp
        0),
    };
    
    void print(std::vector<std::string> vs)
    {
        std::cout<<"{";
        for (auto s : vs) std::cout << s << " ";
        std::cout << "}"<< std::endl;
    }
    void print(std::vector<std::vector<std::string>> vvs)
    {
        std::cout<<"{";
        for (auto vs : vvs)
        {
            std::cout << "{";
                for (auto s : vs) std::cout << s << " ";
                std::cout << "},";
        }
        std::cout<<"}"<<std::endl;
    }
};

TEST_F(SearchTest, searchtest_once)
{
    Search_Once search;
    int count = 1;
    for (auto c : case_list_once)
    {
        std::cout << "case: " << count << std::endl;
        count++;
        search.set_tensors_labels(c.tensors_label);
        search.set_contract_labels(c.contract_label);
        search.run_323_once();
        auto [solution_sequence, min_ntrans3] = search.get_solution();
        EXPECT_EQ(solution_sequence, c.ref_solution_sequence);
        EXPECT_EQ(min_ntrans3, c.ref_min_ntrans3);
        search.clear_all();
    }
}


TEST_F(SearchTest, searchtest_backtrace)
{
    Search_Backtrace search;
    int count = 1;
    for (auto c : case_list_optimized_b01)
    {
        std::cout << "case: " << count << std::endl;
        print(c.tensors_label);
        count++;
        search.set_tensors_labels(c.tensors_label);
        search.set_contract_labels(c.contract_label);
        search.run_323_backtrace();
        auto [solution_sequence, min_ntrans3] = search.get_solution();
        EXPECT_EQ(solution_sequence, c.ref_solution_sequence);
        EXPECT_EQ(min_ntrans3, c.ref_min_ntrans3);
        print(solution_sequence);
        search.clear_all();
    }
    count = 1;
    for (auto c : case_list_optimized_bx2)
    {
        std::cout << "case in bx2:" << count << std::endl;
        print(c.tensors_label);
        count++;
        search.set_tensors_labels(c.tensors_label);
        search.set_contract_labels(c.contract_label);
        search.run_323_backtrace();
        auto [solution_sequence, min_ntrans3] = search.get_solution();
        print(solution_sequence);
        std::cout << "min_ntrans3= " << min_ntrans3 << std::endl;
        search.clear_all();
    }
}