#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <cctype>

#include "parser.h"
#include "code.h"
#include "symboltable.h"

/*
    @2
    0000, 0000, 0000, 0010
    D = A
    1110, 1100, 0001, 0000
    @3
    0000, 0000, 0000, 0011
    D = D + A
    1110, 0000, 1001, 0000
    @0
    0000, 0000, 0000, 0000
    M = D
    1110, 0011, 0000, 1000

    TODO 
    1. 汇编文件解析正常
    2. 汇编语言替换为二进制
    3. 支持符号
*/

bool is_all_digits(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c)) { // 判断字符是否为数字
            return false;
        }
    }
    return true;
}

void write_file(const std::vector<std::string> & ret, const std::string& filename) {
    std::ofstream outfile(filename); // 打开文件

    if (outfile.is_open()) { // 判断文件是否打开成功
        for (const auto& str : ret) {
            outfile << str << std::endl; // 将字符串写入文件
        }
        outfile.close(); // 关闭文件
        std::cout << "Write to file successfully." << std::endl;
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
}

int main(int argc, char** argv) {
    /* 第一阶段处理符号 */
    //const std::string filename("../../add/Add.asm");
    //const std::string filename("../../max/Max.asm");
    const std::string filename(argv[1]);
    std::shared_ptr<Parser> pre_parser = std::make_shared<Parser>(filename);
    std::shared_ptr<Symboltable> symbol_table = std::make_shared<Symboltable>();
  
    while (pre_parser->hasMoreCommands()) {
        pre_parser->advance();

        Parser::command_type ret_type = pre_parser->commandType();
        if (ret_type == Parser::L_COMMAND) {
            auto curr_command = pre_parser->symbol(); 
            std::cout << "pre: " << curr_command << " address:"  << std::to_string(pre_parser->get_pos()) << std::endl;        

            symbol_table->addEntry(curr_command, pre_parser->get_pos());
        } 
    }
   
    std::cout << "=============================== step two ====================================\n";
    std::shared_ptr<Parser> assembler_parser = std::make_shared<Parser>(filename);
    std::shared_ptr<Code> assembler_coder = std::make_shared<Code>();

    std::vector<std::string> ret_binary;
    while (assembler_parser->hasMoreCommands()) {
        assembler_parser->advance();

        Parser::command_type ret_type = assembler_parser->commandType();
        if (ret_type == Parser::A_COMMAND) {
                auto curr_command = assembler_parser->symbol(); 
                std::cout << curr_command << std::endl;

                std::string a_command;
                if (symbol_table->contains(curr_command)) {
                    a_command = symbol_table->GetAddress(curr_command);
                } else if (is_all_digits(curr_command)) {
                    /* 如果数字，那么整数转二进制 */
                    a_command = curr_command;
                } else {
                    /* 变量的情况， 如果没有在符号表，从16开始存放变量，否则直接用变量。 */
                    symbol_table->addEntry(curr_command, symbol_table->GetValuePos());    
                    symbol_table->AddValuePos();
                    a_command = symbol_table->GetAddress(curr_command);
                }
                std::bitset<15> binary(std::stoi(a_command));
                auto ret_string = "0" + binary.to_string();
                std::cout << ret_string << std::endl;
                ret_binary.emplace_back(ret_string);
        } else if (ret_type == Parser::L_COMMAND) {
            /*
                auto curr_command = assembler_parser->symbol(); 
                // 跳转的标签换成整数 
                auto l_command = symbol_table->GetAddress(curr_command);
                std::cout << "debug error L:" << curr_command << " l_command: " << l_command << std::endl;        
                std::bitset<15> binary(std::stoi(l_command));
                auto ret_string = "0" + binary.to_string();
                std::cout << ret_string << std::endl;
                ret_binary.emplace_back(ret_string);
            */
        } else if (ret_type == Parser::C_COMMAND) {
                auto tmp_dest = assembler_parser->dest(); 
                auto tmp_comp = assembler_parser->comp();
                auto tmp_jump = assembler_parser->jump();
                std::cout << tmp_dest << " " << tmp_comp << " " << tmp_jump << std::endl;
                std::string tmp_binary("111");
                tmp_binary += assembler_coder->comp(tmp_comp); 
                tmp_binary += tmp_dest.size() == 0 ? assembler_coder->dest("null") : assembler_coder->dest(tmp_dest); 
                tmp_binary += tmp_jump.size() == 0 ? assembler_coder->jump("null") : assembler_coder->jump(tmp_jump);
                std::cout << tmp_binary << std::endl;
                ret_binary.emplace_back(tmp_binary);
        } else {
            std::cout << "error: " << std::to_string(ret_type) << std::endl;
        }
    }

    write_file(ret_binary, "add.hack");

    return 0;
}