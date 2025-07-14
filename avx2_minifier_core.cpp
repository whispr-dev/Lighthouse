#include "avx2_minifier_core.hpp"
#include <immintrin.h>
#include <string>

std::string minify_json_avx2(const std::string& input) {
    std::string output;
    output.reserve(input.size());

    for (char c : input) {
        if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            output += c;
        }
    }
    return output;
}
