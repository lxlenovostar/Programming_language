#ifndef CODEWRITER 
#define CODEWRITER 

#include <string>
#include <fstream>
#include <vector>

class CodeWriter {
    public:
        CodeWriter(const std::string& filename);
        virtual ~CodeWriter();
        void writeArithmetic(const std::string& command); 
        void writePushPop(const std::string& command, const std::string& segment, int index); 

    private:
        void writeFile(const std::string& command); 

        std::string pushConstant(int value) {
            return setValue(value) + setStackValue() + addStackTop();
        }

        std::string setValue(int value) {
            return "@" + std::to_string(value) + "\nD=A\n";
        }
        
        std::string pushLocal(int value) {
            return setValue(value) + getLocalValue() + setStackValue() + addStackTop();
        }
        
        std::string getLocalValue() {
            return "@LCL\nA=M+D\nD=M\n";
        }
        
        std::string pushThis(int value) {
            return setValue(value) + getThisValue() + setStackValue() + addStackTop();
        }
        
        std::string getThisValue() {
            return "@THIS\nA=M+D\nD=M\n";
        }
 
        std::string pushThat(int value) {
            return setValue(value) + getThatValue() + setStackValue() + addStackTop();
        }
        
        std::string getThatValue() {
            return "@THAT\nA=M+D\nD=M\n";
        }
 
        std::string pushArgument(int value) {
            return setValue(value) + getArgumentValue() + setStackValue() + addStackTop();
        }
        
        std::string getArgumentValue() {
            return "@ARG\nA=M+D\nD=M\n";
        }
        
        std::string pushTemp(int value) {
            return setValue(value) + getTempValue() + setStackValue() + addStackTop();
        }
        
        std::string getTempValue() {
            return "@5\nA=A+D\nD=M\n";
        }
        
        std::string setStackValue() {
            return "@SP\nA=M\nM=D\n";
        }
        
        std::string getStackValue() {
            return "@SP\nA=M-1\nD=M\n";
        }
        
        std::string addStackTop() {
            return "@SP\nM=M+1\n";
        }
  
        std::string subStackTop() {
            return "@SP\nM=M-1\n";
        }

        std::string getStackValueAndSubStackTop() {
            return "@SP\nAM=M-1\nD=M\n";
        }

        std::string getAddressFromR13AndSetValue() {
            return "@R13\nA=M\nM=D\n";
        }

        std::string setValueToR13() {
            return "@R13\nM=D\n";
        } 
        
        std::string getValueToR13() {
            return "@R13\nA=M\nM=D\n";
        }

        std::string popLocal(int value) {
            return setValue(value) + getLocalValue() +
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
 
        std::string popThis(int value) {
            return setValue(value) + getThisValue() +
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
  
        std::string popThat(int value) {
            return setValue(value) + getThatValue() +
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
 
        std::string popArgument(int value) {
            return setValue(value) + getArgumentValue() +
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
        
        std::string getTempAddress() {
            return "@5\nA=A+D\nD=A\n";
        }

        std::string popTemp(int value) {
            return setValue(value) + getTempAddress() +
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }


        std::ofstream m_file;
        int m_index;
};

#endif