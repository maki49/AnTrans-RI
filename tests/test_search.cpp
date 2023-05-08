#include <gtest/gtest.h>
#include "../include/search.hpp"
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
    std::vector<Case> case_list{
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
        // ,
        // Case(   //6
        // { {"a0", "a1", "a2"}, {"a2", "b0"}, {"a1", "b1"}, {"b0", "b1", "b2"} },
        // { "a1", "b0", "a2", "b1" },
        // { "a0", "b2" },
        // { Opts::m01 + "NN", Opts::t3 + Opts::m12 + "TN", Opts::m12 + Opts::to_right + Opts::t3 + Opts::m01 + "NN" },
        // 2)
        //...add more ...
    };

    Search search;
};

TEST_F(SearchTest, searchtest_once)
{
    int count = 1;
    for (auto c : case_list)
    {
        std::cout << "case: " << count << std::endl;
        count++;
        search.set_tensors_labels(c.tensors_label);
        search.set_contract_labels(c.contract_label);
        search.run_323();
        auto [solution_sequence, min_ntrans3] = search.get_solution();
        EXPECT_EQ(solution_sequence, c.ref_solution_sequence);
        EXPECT_EQ(min_ntrans3, c.ref_min_ntrans3);
        search.clear_all();
    }
}