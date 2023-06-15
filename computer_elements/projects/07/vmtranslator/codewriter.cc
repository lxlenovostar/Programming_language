#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "codewriter.h"
#include "parser.h"

CodeWriter::CodeWriter(const std::string& filename): m_file(filename), m_index(0) {
    if (!m_file.is_open()) {
        std::cout << "can't open file: " << filename << std::endl;
    } 
}

CodeWriter::~CodeWriter() {
    if (m_file.is_open()) {
        m_file.close();
    }
}

void CodeWriter::writeFile(const std::string& command) {

    if (m_file.is_open()) { // 判断文件是否打开成功
        m_file << command; // 将字符串写入文件
        std::cout << "Write to file successfully." << std::endl;
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
}

void CodeWriter::writePushPop(const std::string& command, 
    const std::string& segment, int index) {

    /*
        push constant 7

        @7
        D=A   // D中是7 

        @SP
        A=M   // 取得了SP中的地址
        M=D   // SP中的value起作用

        @SP
        M=M+1
    */

    std::string ret_command; 
    if (command == "push") {
        if (segment == "constant") {
            ret_command = pushConstant(index); 
        }
    }
    writeFile(ret_command);


}

void CodeWriter::writeArithmetic(const std::string& command) {
    std::string ret_command; 
    if (command == "add") {
        /*
         add 

         @SP
         A=M-1    // 地址暂时存在A
         D=M      // 取栈顶元素

         @SP
         M=M-1    // SP--

         @SP
         A=M-1
         M=M+D
         */
        ret_command = getStackValue() + subStackTop() + std::string("@SP\nA=M-1\nM=M+D\n"); 

    } else if (command == "sub") {
        /*
         sub 

         @SP
         A=M-1    // 地址暂时存在A
         D=M      // 取栈顶元素

         @SP
         M=M-1    // SP--

         @SP
         A=M-1
         M=M-D
         */
        ret_command = getStackValue() + subStackTop() + std::string("@SP\nA=M-1\nM=M-D\n"); 
    } else if (command == "neg") {
        /*
         neg 

         @SP
         A=M-1
         M=-M
         */
        ret_command = std::string("@SP\nA=M-1\nM=-M\n"); 
    } else if (command == "not") {
        /*
         neg 

         @SP
         A=M-1
         M=!M
         */
        ret_command = std::string("@SP\nA=M-1\nM=!M\n"); 
    } else if (command == "and") {        
        /*
         and 

         @SP
         A=M-1    // 地址暂时存在A
         D=M      // 取栈顶元素

         @SP
         M=M-1    // SP--

         @SP
         A=M-1
         M=M&D
         */
        ret_command = getStackValue() + subStackTop() + std::string("@SP\nA=M-1\nM=M&D\n"); 
    } else if (command == "or") {        
        /*
         or 

         @SP
         A=M-1    // 地址暂时存在A
         D=M      // 取栈顶元素

         @SP
         M=M-1    // SP--

         @SP
         A=M-1
         M=M|D
         */
        ret_command = getStackValue() + subStackTop() + std::string("@SP\nA=M-1\nM=M|D\n"); 
    } else if (command == "eq" || command == "lt" || command == "gt") {
        m_index++;
        std::string inner_command;

        if (command == "eq") {
            inner_command = "JEQ";
        } else if (command == "lt") {
            inner_command = "JLT";
        } else {
            inner_command = "JGT";
        }

        /*
         eq/lt/gt 

         @SP
         A=M-1    // 地址暂时存在A
         D=M      // 取栈顶元素

         @SP
         M=M-1    // SP--

         @SP
         A=M-1
         D=M-D
         @TRUE
         D;JEQ
         @SP
         A=M-1
         M=0
         @END
         0;JMP
         (TRUE)
         @SP
         A=M-1
         M=-1
         (END)
         */
        
        std::string str_index = std::to_string(m_index); 
        ret_command = getStackValue() + subStackTop() + std::string("@SP\nA=M-1\nD=M-D\n@TRUE") 
            + str_index + "\nD;" + inner_command + "\n@SP\nA=M-1\nM=0\n@END" + str_index 
            + "\n0;JMP\n(TRUE" + str_index + ")\n@SP\nA=M-1\nM=-1\n(END" + str_index + ")\n"; 
    }

    writeFile(ret_command);
}
