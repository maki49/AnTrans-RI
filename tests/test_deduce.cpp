#include <gtest/gtest.h>
#include "../include/opts.hpp"
#include "../include/deduce.hpp"
#include "../include/tools.hpp"

struct Case
{
    Case(std::vector<std::string> x, std::vector<std::string> y,
        std::string result, std::string label_mul = "") :
        x(x), y(y), result(result), label_mul(label_mul) {}
    ~Case() {}
    std::vector<std::string> x;
    std::vector<std::string> y;
    std::string result;
    std::string label_mul="";
};
    class DeduceTest : public testing::Test
{
protected:
    std::vector<Case> case_list{
        Case({"a1", "a2", "b0"}, {"a2", "b1"} , Opts::t3 + Opts::m12 + "TN"),
        Case({"a0", "b1", "b0"}, {"b0", "b1", "b2"},  Opts::m12 + Opts::to_right + Opts::t3 + Opts::m01 + "NN", "b1b0"),
        Case({"b0", "a1", "b2"}, {"b0", "b1", "b2"}, Opts::t3 + Opts::m12 + Opts::to_right + Opts::t3 + Opts::m12 + "NT", "b0b2")
    };
    // std::vector<std::vector<std::string>> case32 = { {"a1", "a2", "b0"}, {"a2", "b1"} };    //32TN
    // //std::vector<std::vector<std::string>> case33 = { {"b0", "a1", "b2"}, {"b0", "b1", "b2"} };  //32R32NT
    // std::vector<std::vector<std::string>> case33 = { {"a0", "b1", "b0"}, {"b0", "b1", "b2"} };  //2R31NN
};

TEST_F(DeduceTest, deducetest)
{
    Deduce deduce;
    for (auto c : case_list)
    {
        if (c.y.size() == 2)
        {
            deduce.set_label_mul(tools::find_common_32(c.x, c.y));
            std::string trans32 = deduce.if_trans_32(c.x, c.y);
            if (trans32 == Opts::t3) c.x = Opts::label3_trans(c.x);

            std::string merge32 = deduce.which_merge_32(c.x, c.y);
            c.x = Opts::label3_merge(c.x, merge32);

            std::string gemm32 = deduce.trans_in_gemm(c.x, c.y);

            EXPECT_EQ(trans32 + merge32 + gemm32, c.result);
        }
        else if (c.y.size() == 3)
        {
            assert(c.label_mul != "");
            deduce.set_label_mul(c.label_mul);
            std::string trans_merge_33 = deduce.how_trans_merge_33(c.x, c.y);

            std::vector<std::string>* optmat = &c.x;

            // to do : change this to a subfuntion in Opts: apply_trans_merge_33
            for (const char s : trans_merge_33)
            {
                if (s == Opts::t3[0]) *optmat = Opts::label3_trans(*optmat);
                else if (s == Opts::m01[0]) *optmat = Opts::label3_merge(*optmat, Opts::m01);
                else if (s == Opts::m12[0]) *optmat = Opts::label3_merge(*optmat, Opts::m12);
                else if (s == Opts::to_right[0]) optmat = &c.y;
                //std::cout << "s=" <<s<< std::endl;
            }
            EXPECT_EQ(trans_merge_33 + deduce.trans_in_gemm(c.x, c.y), c.result);
            break;
        }
    }

}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}