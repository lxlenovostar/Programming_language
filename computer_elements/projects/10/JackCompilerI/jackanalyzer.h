#ifndef JACKANALYZER 
#define JACKANALYZER 

#include <iostream>
#include <string>
#include <vector>

class JackAnalyzer {
    public: 
    template<typename Filter>
        std::vector<std::string> GetDicFileByFilter(const std::string& dirName, Filter&& filter) {
                // check the parameter !
    if (dirName.empty())
    {
        std::cout << " dirName is null ! " << std::endl;
        return {};
    }

    // check if dirName is a valid dir
    struct stat s;
    lstat(dirName.c_str(), &s);
    if (!S_ISDIR(s.st_mode))
    {
        std::cout << "dirName is not a valid directory !" << std::endl;
        return {};
    }

    struct dirent *filename;    // return value for readdir()
    DIR *dir;                   // return value for opendir()
    dir = opendir(dirName.c_str());
    if (dir == nullptr)
    {
        std::cout << "Can not open dir " << dirName << std::endl;
        return {};
    }
    std::cout << "Successfully opened the dir !" << std::endl;

    std::vector<std::string> files;
    /* read all the files in the dir ~ */
    while ((filename = readdir(dir)) != nullptr)
    {
        // get rid of "." and ".."
        if (strcmp(filename->d_name, ".") == 0 ||
            strcmp(filename->d_name, "..") == 0)
            continue;
        // std::cout << filename->d_name << std::endl;
        if(filter(filename->d_name))
        {
            files.emplace_back(filename->d_name);
        }
    }
    for(auto& v: files)
    {
        v = dirName + v;
    }
    return files;
        }
    
    JackAnalyzer() = default;
    ~JackAnalyzer() = default;
};

#endif