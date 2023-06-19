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
        } else if (segment == "local") {
            /*
             push local 0

             @0
             D=A        // 0

             @LCL
             A=M+D      // 偏移0
             D=M
     
             @SP
             A=M   // 取得了SP中的地址
             M=D   // SP中的value起作用

             @SP
             M=M+1
            */
            ret_command = pushLocal(index); 
        } else if (segment == "this") {
            ret_command = pushThis(index); 
        } else if (segment == "that") {
            ret_command = pushThat(index); 
        } else if (segment == "temp") {
            /*
             push temp 6

             @6
             D=A        // 6

             @5
             A=A+D      // temp的起始地址是5
             D=M
     
             @SP
             A=M   // 取得了SP中的地址
             M=D   // SP中的value起作用

             @SP
             M=M+1
            */
            ret_command = pushTemp(index); 
        } else if (segment == "argument") {
            ret_command = pushArgument(index); 
        } else if (segment == "pointer") {
            ret_command = pushPointer(index); 
        }
    } else if (command == "pop") {
        if (segment == "local") {
            /*
             pop local 0

             // local 0 的地址存放在 R13 
             // R13 存的是地址

             #@LCL
             #D=M        // 段地址
             #@0
             #D=D+A 
             #@R13
             #M=D

             @0
             D=A
             @LCL
             A=M+D
             D=M

             @R13
             M=D

             @SP
             AM=M-1    // SP--
             D=M       // 取栈顶元素

             @R13
             A=M 
             M=D
             */
            ret_command = popLocal(index); 
        } else if (segment == "this") {
            /*
             pop this 6 

             @6
             D=A
             @THIS
             A=M+D
             D=M

             @R13
             M=D

             @SP
             AM=M-1    // SP--
             D=M       // 取栈顶元素

             @R13
             A=M 
             M=D
             */
            ret_command = popThis(index); 
        } else if (segment == "that") {
            ret_command = popThat(index); 
        } else if (segment == "argument") {
            ret_command = popArgument(index); 
        } else if (segment == "temp") {
            /*
             pop temp 6 

             @6
             D=A    //6

             @5
             A=A+D
             D=A

             @R13
             M=D

             @SP
             AM=M-1    // SP--
             D=M       // 取栈顶元素

             @R13
             A=M 
             M=D
             */
            ret_command = popTemp(index); 
        } else if (segment == "pointer") {
            ret_command = popPointer(index); 
        }


    }

    std::cout << "debug: " << ret_command;
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

         // 方法2
         @SP
         AM=M-1
         D=M
         A=A-1
         M=M+D

         */
        ret_command = getStackValue() + std::string("M=M+D\n"); 

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
        ret_command = getStackValue() + std::string("M=M-D\n"); 
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
        ret_command = getStackValue() + std::string("M=M&D\n"); 
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
        ret_command = getStackValue() + std::string("M=M|D\n"); 
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
        ret_command = getStackValue() + std::string("@SP\nA=M-1\nD=M-D\n@TRUE") 
            + str_index + "\nD;" + inner_command + "\n@SP\nA=M-1\nM=0\n@END" + str_index 
            + "\n0;JMP\n(TRUE" + str_index + ")\n@SP\nA=M-1\nM=-1\n(END" + str_index + ")\n"; 
    }

    std::cout << "debug " << ret_command << std::endl;

    writeFile(ret_command);
}
