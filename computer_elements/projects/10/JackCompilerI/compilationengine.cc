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
    
    m_tokener = std::make_shared<JackTokenizer>(m_src_file);
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
    writeFile(ret_command);
}

void CompilationEngine::compileTerm() {
}

void CompilationEngine::compileExpressionList() {
}

void CompilationEngine::compileExpression() {
}

void CompilationEngine::compileIf() {
    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token);   
            if (token == "if" || token == "else") {
                compileIf();
            } else if (token == "let") {
                compileLet();
            } else if (token == "var") {
                compileVarDec();
            } else if (token == "while") {
                compileWhile();
            }

        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == '}') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            auto token = std::to_string(m_tokener->intVal());
            writeToken("integerConstant", token);
        } else if (token_type == JackTokenizer::STRING_CONST) {
            auto token = m_tokener->stringVal();
            writeToken("stringConstant", token);
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    }
}

void CompilationEngine::compileReturn() {
}

void CompilationEngine::compileWhile() {  

    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token);   
            if (token == "if" || token == "else") {
                compileIf();
            } else if (token == "let") {
                compileLet();
            } else if (token == "var") {
                compileVarDec();
            } else if (token == "while") {
                compileWhile();
            }

        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == '}') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            auto token = std::to_string(m_tokener->intVal());
            writeToken("integerConstant", token);
        } else if (token_type == JackTokenizer::STRING_CONST) {
            auto token = m_tokener->stringVal();
            writeToken("stringConstant", token);
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    }

}

void CompilationEngine::compileLet() {
    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token); 
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == ';') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            auto token = std::to_string(m_tokener->intVal());
            writeToken("integerConstant", token);
        } else if (token_type == JackTokenizer::STRING_CONST) {
            auto token = m_tokener->stringVal();
            writeToken("stringConstant", token);
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    }

}

void CompilationEngine::compileDo() {
    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token); 
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == ';') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            auto token = std::to_string(m_tokener->intVal());
            writeToken("integerConstant", token);
        } else if (token_type == JackTokenizer::STRING_CONST) {
            auto token = m_tokener->stringVal();
            writeToken("stringConstant", token);
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    }
    
}

void CompilationEngine::compileStatements() {
}

void CompilationEngine::compileVarDec() {
    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token); 
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == ';') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::STRING_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    
    }

}

void CompilationEngine::compileParameterList() {
}

void CompilationEngine::compileSubroutine() { 
    
    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token);
            if (token == "if" || token == "else") {
                compileIf();
            } else if (token == "let") {
                compileLet();
            } else if (token == "var") {
                compileVarDec();
            } else if (token == "while") {
                compileWhile();
            }
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == '}') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
        } else if (token_type == JackTokenizer::STRING_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    
    }

}

void CompilationEngine::compileClassVarDec() {

    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            writeToken("keyword", token); 
        } else if (token_type == JackTokenizer::SYMBOL) {
           auto token = m_tokener->symbol();
            writeToken("symbol", token);
           if (token[0] == ';') {
                break;
           } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            auto token = std::to_string(m_tokener->intVal());
            writeToken("integerContstant", token);
        } else if (token_type == JackTokenizer::STRING_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    
    }
}

void CompilationEngine::compileClass() {
    writeFile("<tokens>\n");

    while(m_tokener->hasMoreTokens()) {
        m_tokener->advance();

        auto token_type = m_tokener->tokenType();

        if (token_type == JackTokenizer::KEYWORD) {
            auto token = m_tokener->keyword();
            if (token == "static" || token == "field") {
                writeToken("keyword", token);
                compileClassVarDec();
            } else if (token == "constructor" || token == "method" || token == "function") {
                writeToken("keyword", token);
                compileSubroutine();
            } else {
                writeToken("keyword", token);
            }

        } else if (token_type == JackTokenizer::SYMBOL) {
            auto token = m_tokener->symbol();
            writeToken("symbol", token);
            if (token[0] == '}') {
                break;
            } 
        } else if (token_type == JackTokenizer::INT_CONST) {
            // do nothing
        } else if (token_type == JackTokenizer::STRING_CONST) {
        } else if (token_type == JackTokenizer::IDENTIFIER) {
            auto token = m_tokener->identifier();
            writeToken("identifier", token);
        }
    
    }
    writeFile("</tokens>\n");
}