#pragma once
#include <vector>
#include <string>
#include <map>
#include <assert.h>

namespace Opts
{
    std::map<const std::string, const std::string> encoding = 
    {
        {"tensor3_transpose", "3"},
        { "tensor3_merge_01", "1" },
        { "tensor3_merge_12", "2" },
        { "swap", "R" },
        { "to_right", "R" },
        //{ "to_left", "L" }
        { "failed", "F" }
    };

    const std::string& m01 = encoding["tensor3_merge_01"];
    const std::string& m12 = encoding["tensor3_merge_12"];
    const std::string& t3 = encoding["tensor3_transpose"];
    const std::string& swp = encoding["swap"];
    const std::string& to_right = encoding["to_right"];
    //const std::string& to_left = encoding["to_left"];
    const std::string& failed = encoding["failed"];

    const std::vector<std::string> label3_trans(const std::vector<std::string>& l3)
    {
        assert(l3.size() == 3);
        return std::vector<std::string>{l3[1], l3[0], l3[2]};
    }
    
    const std::vector<std::string> label3_merge(const std::vector<std::string>& l3, const std::string pos)
    {
        assert(l3.size() == 3);
        if (pos == m01)
            return std::vector<std::string>{l3[0] + l3[1], l3[2]};
        else if (pos == m12)
            return std::vector<std::string>{l3[0], l3[1] + l3[2]};
        else
            throw std::invalid_argument("invalid pos");
    }

    const std::vector<std::string> label22_contract(const std::vector<std::string>& l2x, const std::vector<std::string>& l2y, const std::string& tn)
    {
        assert(l2x.size() == 2);
        assert(l2y.size() == 2);
        if (tn == "NN" && l2x[1] == l2y[0])
            return std::vector<std::string>{l2x[0], l2y[1]};
        else if (tn == "TT" && l2x[0] == l2y[1])
            return std::vector<std::string>{l2x[1], l2y[0]};
        else if (tn == "NT" && l2x[1] == l2y[1])
            return std::vector<std::string>{l2x[0], l2y[0]};
        else if (tn == "TN" && l2x[0] == l2y[0])
            return std::vector<std::string>{l2x[1], l2y[1]};
        else
            throw std::invalid_argument("invalid gemm type");
    }

    const std::vector<std::string> label2_split(const std::vector<std::string>& l2)
    {
        assert(l2.size() == 2);
        size_t len0 = l2[0].length();
        size_t len1 = l2[1].length();
        if (len0 == 2 * len1)
            return std::vector<std::string>{l2[0].substr(0, len1), l2[0].substr(len1, len1), l2[1]};
        else if (len1 == 2 * len0)
            return std::vector<std::string>{l2[0], l2[1].substr(0, len0), l2[1].substr(len0, len0)};
        else
            throw std::invalid_argument("invalid label length to split");
    }

    void label33_trans_merge(std::vector<std::string>& l3x, std::vector<std::string>& l3y,
        std::vector<std::string>* opt_now, const std::string& trans3_merge)
    {
        for (const char s : trans3_merge)
        {
            if (s == t3[0]) *opt_now = label3_trans(*opt_now);
            else if (s == m01[0]) *opt_now = label3_merge(*opt_now, m01);
            else if (s == m12[0]) *opt_now = label3_merge(*opt_now, m12);
            else if (s == to_right[0]) opt_now = &l3y;
            std::cout << "s=" <<s<< std::endl;
        }
    }
}

    
    
