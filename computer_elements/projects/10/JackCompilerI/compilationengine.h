#ifndef COMPILATIONENGINE 
#define COMPILATIONENGINE 

#include <string>
#include <fstream>
#include <vector>

class CompilationEngine {
    public:
        CompilationEngine(const std::string& src_filename, const std::string& dst_filename);
        virtual ~CompilationEngine();
        void compileClass();
        void compileClassVarDec();
        void compileSubroutine();
        void compileParameterList();
        void compileVarDec();
        void compileStatements();
        void compileDo();
        void compileLet();
        void compileWhile();
        void compileReturn();
        void compileIf();
        void compileExpression();
        void compileTerm();
        void compileExpressionList();
    
    private:
        std::string m_src_file;
        std::ofstream m_dst_file;

        void writeFile(const std::string& command); 
        void writeToken(const std::string& category, const std::string& command); 
};

#endif