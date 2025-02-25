#pragma once
#include <cstring>
#include <vector>
#include <string>
#include <sstream>

class StringUtils {
    public:
        static std::string trim(const std::string& str) {
            std::size_t start = 0;
            while (start < str.size() && std::isspace((unsigned char)str[start])) {
                ++start;
            }
            std::size_t end = str.size();
            while (end > start && std::isspace((unsigned char)str[end - 1])) {
                --end;
            }
            return str.substr(start, end - start);
        }

        static std::vector<std::string> split(const char* str, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(str);
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }
};