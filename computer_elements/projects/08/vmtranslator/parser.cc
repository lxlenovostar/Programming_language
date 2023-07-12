#include <fstream>
#include <algorithm>
#include <iostream>

#include "parser.h"
#include "utils.h"

Parser::Parser(const std::string& filename): m_pos(0), m_curr_line(""), m_assembler_file(filename),
    m_type(NULL_COMMAND) {
    if (!m_assembler_file.is_open()) {
        std::cout << "can't open file: " << filename << std::endl;
    } else {
        preHandleFile();
    }
}

void Parser::preHandleFile() {
    std::string tmp_line("");
    while (std::getline(m_assembler_file, tmp_line)) {
        
        tmp_line.erase(std::remove(tmp_line.begin(), tmp_line.end(), '\r'), tmp_line.end());
        tmp_line.erase(std::remove(tmp_line.begin(), tmp_line.end(), '\n'), tmp_line.end());

        // TODO 处理尾部注释
        if ((tmp_line.find("//") != std::string::npos) || tmp_line.size() == 0) {
            // 去掉注释
            continue;
        }

        std::cout << "debug tmp_line:" << tmp_line << "\n";

        m_command.emplace_back(tmp_line);
    }
}

Parser::~Parser() {
    if (m_assembler_file.is_open()) {
        m_assembler_file.close();
    }
}

bool Parser::hasMoreCommands() {
    return m_assembler_file.is_open() && (m_pos < m_command.size());
}

void Parser::advance() {
    if (m_pos < m_command.size()) {
        m_curr_line = m_command[m_pos];
        ++m_pos;

        m_curr_word = split_string(m_curr_line);
        const std::string& first_word = m_curr_word[0];
        if (first_word == "push") {
            m_type = C_PUSH;
        } else if (first_word == "pop") {
            m_type = C_POP;
        } else if (first_word == "label") {
            m_type = C_LABEL;
        } else if (first_word == "goto") {
            m_type = C_GOTO;
        } else if (first_word == "if-goto") {
            m_type = C_IF;
        } else if (first_word == "function") {
            m_type = C_FUNCTION;
        } else if (first_word == "call") {
            m_type = C_CALL;
        } else if (first_word == "return") {
            m_type = C_RETURN;
        } else {
            m_type = C_ARITHMETIC;
        }

    }
}
    
Parser::command_type Parser::commandType() {
    return m_type;
}

std::string Parser::arg1() {
    if (m_type == C_RETURN)
        return "";
    
    if (m_type == C_ARITHMETIC)
        return m_curr_word[0];
    
    // TODO check m_curr_word
    return m_curr_word[1];
}

int Parser::arg2() {
    if (m_type == C_PUSH || m_type == C_POP || m_type == C_FUNCTION || m_type == C_CALL) {
        return  std::stoi(m_curr_word[2]);      
    }

    // TODO error
    return -1;
}

