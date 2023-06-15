#ifndef UTILS 
#define UTILS 

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> split_string(const std::string& str) {
    std::vector<std::string> result;
    std::string word;
    for (const auto& c : str) {
        if (c == ' ') { // 以空格为分隔符
            if (!word.empty()) { // 如果单词不为空，则添加到结果中
                result.push_back(word);
                word.clear();
            }
        } else {
            word += c; // 否则将字符添加到单词中
        }
    }
    if (!word.empty()) { // 处理最后一个单词
        result.push_back(word);
    }
    return result;
}

#endif