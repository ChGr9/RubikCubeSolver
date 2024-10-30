#pragma once
#include <vector>

class VectorUtils {
public:
    static int indexOf(const std::vector<int>& vec, int value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end()) {
            return std::distance(vec.begin(), it);
        }
        else {
            return -1;
        }
    }
};