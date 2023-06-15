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

        std::ofstream m_file;
        int m_index;

};

#endif