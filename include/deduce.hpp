#include<string>
#include<vector>
#include <assert.h>
#include <sstream>
#include "opts.hpp"

class Deduce
{

public:

    Deduce(std::string label_mul) : label_mul(label_mul) {}
    Deduce() {};
    ~Deduce() {};

    void set_label_mul(std::string l) { this->label_mul = l; }
    const std::string  get_label_mul() { return this->label_mul; }

    const std::string trans_in_gemm(const std::vector<std::string>& l2x, const std::vector<std::string>& l2y)
    {
        assert(l2x.size() == 2);
        assert(l2y.size() == 2);

        if (l2x[1] == l2y[0] && this->label_mul == l2x[1]) return "NN";
        else if (l2x[0] == l2y[1] && this->label_mul == l2x[0]) return "TT";
        else if (l2x[1] == l2y[1] && this->label_mul == l2x[1]) return "NT";
        else if (l2x[0] == l2y[0] && this->label_mul == l2x[0]) return "TN";
        else return Opts::failed;
    }

    const std::string which_merge_32(const std::vector<std::string>& l3x, const std::vector<std::string>& l2y)
    {
        assert(l3x.size() == 3);
        assert(l2y.size() == 2);
        if (l3x[0] == l2y[0] || l3x[0] == l2y[1] && l3x[0] == this->label_mul) return Opts::m12;
        else if (l3x[2] == l2y[0] || l3x[2] == l2y[1] && l3x[2] == this->label_mul) return Opts::m01;
        else return Opts::failed;
    }

    const std::string if_trans_32(const std::vector<std::string>& l3x, const std::vector<std::string>& l2y) const
    {
        assert(l3x.size() == 3);
        assert(l2y.size() == 2);
        if (l3x[1] == l2y[0] || l3x[1] == l2y[1])
        {
            assert(this->label_mul == l3x[1]);
            return Opts::t3;
        }
        else return "";
    }

    const std::string how_trans_merge_33(const std::vector<std::string>& l3x, const std::vector<std::string>& l3y) const
    {
        assert(l3x.size() == 3);
        assert(l3y.size() == 3);
        assert(this->label_mul.length() == 2 * label_len);
        std::string x = l3x[0] + l3x[1] + l3x[2];
        std::string y = l3y[0] + l3y[1] + l3y[2];
        size_t ix = x.find(this->label_mul);
        size_t iy = y.find(this->label_mul);

        bool ltrans = false;  bool rtrans = false;

        auto which_merge_33 = [&]()->std::string
        {
            std::stringstream ss;
            if (ltrans) ss << Opts::t3;
            if (ix == 0) ss << Opts::m01;
            else ss << Opts::m12;
            ss << Opts::to_right;
            if (rtrans) ss << Opts::t3;
            if (iy == 0) ss << Opts::m01;
            else ss << Opts::m12;
            return ss.str();
        };

        // both find, directly merge
        if (ix != std::string::npos && iy != std::string::npos) return which_merge_33();

        // deal with not-found
        std::string label_first = label_mul.substr(0, label_len);
        if (ix == std::string::npos)
        {   //ab not in x, x must in {bac, bca, acb, cba}, where {bac, acb} can be solved by trans
            if (label_first == l3x[2]) return Opts::failed;
            ltrans = true;
            if (label_first == l3x[0]) ix = static_cast<size_t>(1);   //acb
            else if (label_first == l3x[1]) ix = static_cast<size_t>(0);   //bac
            else throw std::invalid_argument("label missmatched");
        }
        if (iy == std::string::npos)
        {   // same as ix
            if (label_first == l3y[2]) return Opts::failed;
            rtrans = true;
            if (label_first == l3y[0]) iy = static_cast<size_t>(1);   //acb
            else if (label_first == l3x[1]) iy = static_cast<size_t>(0);   //bac
            else throw std::invalid_argument("label missmatched");
        }
        return which_merge_33();
    }

private:

    std::string label_mul;  //Current constracted label. For 3*3, loop 2 order (ab, ba) outside.
    const int label_len = 2; //"a0"

};