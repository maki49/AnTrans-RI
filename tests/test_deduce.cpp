#include <gtest/gtest.h>
#include "../include/opts.hpp"
#include "../include/deduce.hpp"

class DeduceTest : public testing::Test
{
protected:
    std::vector<std::vector<std::string>> case32 = { {"a1", "a2", "b0"}, {"a2", "b1"} };    //32TN
    std::vector<std::vector<std::string>> case33 = { {"b0", "a1", "b2"}, {"b0", "b1", "b2"} };  //32R32NT
};

TEST_F(DeduceTest, deducetest)
{
    Deduce deduce("a2");

    //case32
    std::string trans32 = deduce.if_trans_32(case32[0], case32[1]);
    if (trans32 == Opts::t3) case32[0] = Opts::label3_trans(case32[0]);

    std::string merge32 = deduce.which_merge_32(case32[0], case32[1]);
    case32[0] = Opts::label3_merge(case32[0], merge32);

     std::string gemm32 = deduce.trans_in_gemm(case32[0], case32[1]);
     std::cout << "gemm32=" << gemm32 << std::endl;
     EXPECT_EQ(trans32 + merge32 + gemm32, Opts::t3 + Opts::m12 + "TN");

    //case 33
    deduce.set_label_mul("b0b2");
    std::string trans_merge_33 = deduce.how_trans_merge_33(case33[0], case33[1]);
    EXPECT_EQ(trans_merge_33, Opts::t3 + Opts::m12 + Opts::to_right + Opts::t3 + Opts::m12);

    std::vector<std::string> *optmat = &case33[0];
    for (const char s : trans_merge_33)
    {
        if (s == Opts::t3[0]) *optmat = Opts::label3_trans(*optmat);
        else if (s == Opts::m01[0]) *optmat = Opts::label3_merge(*optmat, Opts::m01);
        else if (s == Opts::m12[0]) *optmat = Opts::label3_merge(*optmat, Opts::m12);
        else if (s == Opts::to_right[0]) optmat = &case33[1];
        std::cout << "s=" <<s<< std::endl;
    }
    EXPECT_EQ(deduce.trans_in_gemm(case33[0], case33[1]), "NT");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}