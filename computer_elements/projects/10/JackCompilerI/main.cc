#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <dirent.h>
#include <algorithm>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"
#include "jackanalyzer.h"
#include "compilationengine.h"

int main(int argc, char** argv) {
    const std::string& path(argv[1]);

    std::shared_ptr<JackAnalyzer> parser = std::make_shared<JackAnalyzer>();

    auto v = parser->GetDicFileByFilter(path,
                                [](const std::string &s) {
                                    // std::cout << s.substr(s.size() -3) << std::endl;
                                    if (s.substr(s.size() - 5) == ".jack")
                                    { return true; }
                                    else
                                    { return false; }
                                });


    for (auto &val:v) {
        std::cout << "##################################### debug filename:" << val << std::endl;
        // TODO 修改模板文件名
        std::string dst_filename = get_filename(val);
        // 因为写入为append, 先删除文件
        remove(dst_filename.c_str());

        std::shared_ptr<CompilationEngine> engine = std::make_shared<CompilationEngine>(val, dst_filename+".xml");
        engine->compileClass();
    }

    return 0;
}