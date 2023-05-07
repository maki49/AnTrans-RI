#include <vector>
#include <string>
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
}

    
    
