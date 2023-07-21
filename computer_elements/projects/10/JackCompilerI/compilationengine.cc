#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "utils.h"
#include "compilationengine.h"
#include "jacktokenizer.h"

CompilationEngine::CompilationEngine(const std::string& src_filename, const std::string& dst_filename): 
m_src_file(src_filename), m_dst_file(dst_filename, std::ios::app) {
    if (!m_dst_file.is_open()) {
        std::cout << "can't open file: " << dst_filename << std::endl;
    } 

}

CompilationEngine::~CompilationEngine() {
    if (m_dst_file.is_open()) {
        m_dst_file.close();
    }
}

void CompilationEngine::writeFile(const std::string& command) {
    if (m_dst_file.is_open()) { // 判断文件是否打开成功
        m_dst_file << command; // 将字符串写入文件
        std::cout << "Write to file successfully." << std::endl;
    } else {
        std::cout << "Failed to open file." << std::endl;
    }
}

void CompilationEngine::writeToken(const std::string& category, const std::string& command) {
    std::string ret_command = "<" + category + "> " + command + " </" + category + ">\n";
    writeFile(command);
}

void CompilationEngine::compileTerm() {
}

void CompilationEngine::compileExpressionList() {
}

void CompilationEngine::compileExpression() {
}

void CompilationEngine::compileIf() {
}

void CompilationEngine::compileReturn() {
}

void CompilationEngine::compileWhile() {
}

void CompilationEngine::compileLet() {
}

void CompilationEngine::compileDo() {
}

void CompilationEngine::compileStatements() {
}

void CompilationEngine::compileVarDec() {
}

void CompilationEngine::compileParameterList() {
}

void CompilationEngine::compileSubroutine() {
}

void CompilationEngine::compileClassVarDec() {
    /*
    writeFile("<tokens>\n");
    std::shared_ptr<JackTokenizer> tokens = std::make_shared<JackTokenizer>(m_dst_file);

    while(tokens->hasMoreTokens()) {
        tokens->advance();

        auto token_type = tokens->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = tokens->keyword();
            if (token == "class") {
                writeToken("keyword", token);
            } else if (token == "static" || token == "field") {
                writeToken("keyword", token);
                compileClassVarDec();
            } else if (token == "constructor" || token == "method" || token == "function") {
                writeToken("keyword", token);
                compileSubroutine();
            }
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = tokens->symbol();
           if (token[0] == '}') {
                writeToken("symbol", token);
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::STRING_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = tokens->identifier();
            writeToken("identifier", token);
        }
    
    }
    writeFile("</tokens>\n");
    */
}
void CompilationEngine::compileClass() {
    writeFile("<tokens>\n");
    std::shared_ptr<JackTokenizer> tokens = std::make_shared<JackTokenizer>(m_src_file);

    while(tokens->hasMoreTokens()) {
        tokens->advance();

        auto token_type = tokens->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = tokens->keyword();
            if (token == "class") {
                writeToken("keyword", token);
            } else if (token == "static" || token == "field") {
                writeToken("keyword", token);
                compileClassVarDec();
            } else if (token == "constructor" || token == "method" || token == "function") {
                writeToken("keyword", token);
                compileSubroutine();
            }
        } else if (token_type == JackTokenizer::SYMBOL) {
            auto token = tokens->symbol();
            writeToken("symbol", token);
            if (token[0] == '}') {
                break;
            } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::STRING_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = tokens->identifier();
            writeToken("identifier", token);
        }
    
    }
    writeFile("</tokens>\n");
}