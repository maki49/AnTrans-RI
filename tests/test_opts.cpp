#include <gtest/gtest.h>
#include "../include/opts.hpp"
#include <vector>
#include <string>

class OptsTest : public testing::Test
{
protected:
    std::vector<std::string> test_vec = { "a1", "b0", "b1" };
};

TEST_F(OptsTest, optstest)
{
    // trans
    std::vector<std::string>  trans = Opts::label3_trans(test_vec);
    EXPECT_EQ(trans[0], "b0");
    EXPECT_EQ(trans[1], "a1");
    EXPECT_EQ(trans[2], "b1");

    //merge
    std::vector<std::string> merge1 = Opts::label3_merge(test_vec, Opts::m01);
    EXPECT_EQ(merge1[0], "a1b0");
    EXPECT_EQ(merge1[1], "b1");
    
    std::vector<std::string> merge2 = Opts::label3_merge(test_vec, Opts::m12);
    EXPECT_EQ(merge2[0], "a1");
    EXPECT_EQ(merge2[1], "b0b1");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}