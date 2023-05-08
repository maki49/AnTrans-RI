#include <string>
#include <vector>
#include "../include/opts.hpp"
namespace tools
{
    std::string find_common(const std::vector<std::string>& v1, const std::vector<std::string>& v2)
    {
        for (auto s1 : v1)
            for (auto s2 : v2)
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