#ifndef UTILS_H 
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>

std::string get_filename(const std::string& full_file_name); 
std::vector<std::string> split_string(const std::string& str);

#endif