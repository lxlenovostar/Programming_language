#include <fstream>
#include <algorithm>
#include <iostream>

#include "parser.h"

Parser::Parser(const std::string& filename): m_pos(0), m_rom_pos(0), m_curr_line(""), m_assembler_file(filename), m_curr_dest(""),
     m_curr_comp(""), m_curr_jump(""), m_type(NULL_COMMAND) {
    if (!m_assembler_file.is_open()) {
        std::cout << "can't open file: " << filename << std::endl;
    } else {
        preHandleFile();
    }
}

void Parser::preHandleFile() {
    std::string tmp_line("");
    while (std::getline(m_assembler_file, tmp_line)) {
    
       // 去掉空格
       tmp_line.erase(std::remove_if(tmp_line.begin(), tmp_line.end(),
            [](char c) { return std::isspace(c); }), tmp_line.end());
        
        // TODO 处理尾部注释
        if ((tmp_line.find("//") != std::string::npos) || tmp_line.size() == 0) {
            // 去掉注释
            continue;
        }

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

        m_curr_dest.clear();
        m_curr_comp.clear();
        m_curr_jump.clear();  
        
        switch (m_curr_line[0]) {
            case '@':
                m_type = A_COMMAND;
                ++m_rom_pos;
                break;
            case '(':
                m_type = L_COMMAND;
                break;
            default:
                m_type = C_COMMAND;
                ++m_rom_pos;
                handleCCommand();
                break;
        }
    }
}
    
Parser::command_type Parser::commandType() {
    return m_type;
}

void Parser::handleCCommand() {
    auto pos1 = m_curr_line.find_first_of('=');
    auto pos2 = m_curr_line.find_first_of(';');

    if (pos1 != std::string::npos) {
        // 取 dest 
        size_t pos_len = pos1;
        m_curr_dest = m_curr_line.substr(0, pos_len);
    }

    if (pos2 != std::string::npos) {
        // 取 jump 
        size_t pos_len = m_curr_line.size() - pos2 - 1;
        m_curr_jump = m_curr_line.substr(pos2 + 1, pos_len);
    }
    
    // C 指令一定有comp , 取 comp 
    pos1 = pos1 == std::string::npos ? 0 : pos1 + 1;
    pos2 = pos2 == std::string::npos ? m_curr_line.size() - 1 : pos2 - 1;
    m_curr_comp = m_curr_line.substr(pos1, (pos2 - pos1 + 1));
}

std::string Parser::symbol() {
    if (m_type == A_COMMAND) {
        // -1 代表去掉 @
        return m_curr_line.substr(1, m_curr_line.size() - 1);
    }

    if (m_type == L_COMMAND) {
        // -2 代表去掉  ()
        return m_curr_line.substr(1, m_curr_line.size() - 2);
    }
    
    return "";
}

std::string Parser::dest() {
    if (m_type != C_COMMAND) {
        return "";
    } 

    return m_curr_dest;
}
    
std::string Parser::comp() {
    if (m_type != C_COMMAND) {
        return "";
    } 

    return m_curr_comp;
}
    
std::string Parser::jump() {
    if (m_type != C_COMMAND) {
        return "";
    } 
    
    return m_curr_jump;
}
