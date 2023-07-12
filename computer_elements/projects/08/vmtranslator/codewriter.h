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
            return setValueContent(value) + setStackValue() + addStackTop();
        }

        std::string setValueContent(int value) {
            return "@" + std::to_string(value) + "\nD=A\n";
        }
        
        std::string setValueValue(int value) {
            return "@" + std::to_string(value) + "\nD=D+A\n";
        }
        
        std::string setValueAddress(int value) {
            return "@" + std::to_string(value) + "\nA=D+A\nD=M\n";
        }
        
        std::string pushStatic(int value) {
            return "@" + m_file_name + "." + std::to_string(value) + "\nD=M\n" + setStackValue() + addStackTop();
        }

        std::string pushLocal(int value) {
            return getLocalValue() + setValueAddress(value) + setStackValue() + addStackTop();
        }
        
        std::string getLocalValue() {
            return "@LCL\nD=M\n";
        }
        
        std::string pushThis(int value) {
            return getThisValue() + setValueAddress(value) + setStackValue() + addStackTop();
        }
        
        std::string getThisValue() {
            return "@THIS\nD=M\n";
        }
        
        std::string getThisAdress() {
            return "@THIS\nD=A\n";
        }
 
        std::string pushThat(int value) {
            return getThatValue() + setValueAddress(value) + setStackValue() + addStackTop();
        }

        std::string pushPointer(int value) {
            const std::string& begin = value == 0 ? getThisValue() : getThatValue();
            return begin + setStackValue() + addStackTop();
        }
        
        std::string getThatValue() {
            return "@THAT\nD=M\n";
        }
        
        std::string getThatAddress() {
            return "@THAT\nD=A\n";
        }
 
        std::string pushArgument(int value) {
            return getArgumentValue() + setValueAddress(value) + setStackValue() + addStackTop();
        }
        
        std::string getArgumentValue() {
            return "@ARG\nD=M\n";
        }
        
        std::string pushTemp(int value) {
            return getTempValue() + setValueAddress(value) + setStackValue() + addStackTop();
        }
        
        std::string getTempValue() {
            return "@R5\nD=A\n";
        }
        
        std::string setStackValue() {
            return "@SP\nA=M\nM=D\n";
        }
        
        std::string getStackValue() {
            return "@SP\nAM=M-1\nD=M\nA=A-1\n";
        }
        
        std::string addStackTop() {
            return "@SP\nM=M+1\n";
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
 
        std::string popStatic(int value) {
            return getStackValueAndSubStackTop() + "@" + m_file_name + "." + std::to_string(value)
                + "\nM=D\n";
        }

        std::string popLocal(int value) {
            return getLocalValue() + setValueValue(value) + 
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
 
        std::string popThis(int value) {
            return getThisValue() + setValueValue(value) + 
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
  
        std::string popThat(int value) {
            return getThatValue() + setValueValue(value) + 
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
        
        /* TODO Pointer的作用理解不深刻 */
        std::string popPointer(int value) {
            const std::string& begin = value == 0 ? getThisAdress() : getThatAddress();
            return begin + setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
 
        std::string popArgument(int value) {
            return getArgumentValue() + setValueValue(value) + 
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }
        
        std::string getTempAddress() {
            return "@R5\nD=A\n";
        }

        std::string popTemp(int value) {
            return getTempAddress() + setValueValue(value) + 
                setValueToR13() + getStackValueAndSubStackTop() +
                getValueToR13();
        }


        std::ofstream m_file;
        std::string m_file_name;
        int m_index;
};

#endif