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
        0)
    //...add more ...
    };

    Search search;
};

TEST_F(SearchTest, searchtest)
{
    for (auto c : case_list)
    {
        search.set_tensors_labels(c.tensors_label);
        search.set_contract_labels(c.contract_label);
        search.run_323();
        auto [solution_sequence, min_ntrans3] = search.get_solution();
        EXPECT_EQ(solution_sequence, c.ref_solution_sequence);
        EXPECT_EQ(min_ntrans3, c.ref_min_ntrans3);
    }
}