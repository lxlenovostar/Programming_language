#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include "parser.h"
#include "codewriter.h"

/* Show all files under dir_name , do not show directories ! */
template<typename Filter>
std::vector<std::string> GetDicFileByFilter(const std::string& dirName, Filter&& filter)
{
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

int main(int argc, char** argv) {
    const std::string& path(argv[1]);
    const std::string& outfilename(argv[2]);

    // 因为写入为append, 先删除文件
    remove(argv[2]);


    auto v = GetDicFileByFilter(path,
                                [](const std::string &s) {
                                    // std::cout << s.substr(s.size() -3) << std::endl;
                                    if (s.substr(s.size() - 3) == ".vm")
                                    { return true; }
                                    else
                                    { return false; }
                                });


    for (auto &val : v) {
        bool init_flag = false;
        if (val.find("Sys.vm") != std::string::npos) {
            init_flag = true;
            std::shared_ptr<CodeWriter> codewriter = std::make_shared<CodeWriter>(outfilename, val, init_flag);
        }
    }



    for (auto &val:v) {
        std::cout << "##################################### debug filename:" << val << std::endl;
        std::shared_ptr<Parser> parser = std::make_shared<Parser>(val);
        std::shared_ptr<CodeWriter> codewriter = std::make_shared<CodeWriter>(outfilename, val, false);

        while (parser->hasMoreCommands()) {
            parser->advance();

            std::cout << "debug: " << parser->getCurrLine() << std::endl;

            Parser::command_type ret_type = parser->commandType();
            if (ret_type == Parser::C_PUSH) {
                codewriter->writePushPop("push", parser->arg1(), parser->arg2());
            } else if (ret_type == Parser::C_POP) {
                codewriter->writePushPop("pop", parser->arg1(), parser->arg2());
            } else if (ret_type == Parser::C_ARITHMETIC) {
                codewriter->writeArithmetic(parser->arg1());
            } else if (ret_type == Parser::C_LABEL) {
                codewriter->writeLabel(parser->arg1());
            } else if (ret_type == Parser::C_IF) {
                codewriter->WriteIf(parser->arg1());
            } else if (ret_type == Parser::C_GOTO) {
                codewriter->WriteGoto(parser->arg1());
            } else if (ret_type == Parser::C_FUNCTION) {
                codewriter->WriteFunction(parser->arg1(), parser->arg2());
            } else if (ret_type == Parser::C_RETURN) {
                codewriter->WriteReturn();
            } else if (ret_type == Parser::C_CALL) {
                codewriter->WriteCall(parser->arg1(), parser->arg2());
            }

        }
    }

    return 0;
}