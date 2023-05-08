#include <string>
#include <vector>
#include <assert.h>
#include "../include/opts.hpp"
namespace tools
{
    std::string find_common_32(const std::vector<std::string>& l3x, const std::vector<std::string>& l2y)
    {
        assert(l3x.size() == 3);
        assert(l2y.size() == 2);
        for (auto s1 : l3x)
            for (auto s2 : l2y)
                if (s1 == s2) return s1;
        return Opts::failed;
    }

    int count_char(const std::string& s, const char c)
    {
        int count = 0;
        for (auto ch : s)
            if (ch == c) count++;
        return count;
    }
}