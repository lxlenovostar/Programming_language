#include <fstream>
#include <algorithm>
#include <iostream>

#include "jacktokenizer.h"
#include "utils.h"

JackTokenizer::JackTokenizer(const std::string& filename): m_pos(-1), m_jack_file(filename),
    m_type(NULL_TYPE), m_tokens_len(-1) {
    if (!m_jack_file.is_open()) {
        std::cout << "can't open file: " << filename << std::endl;
    } else {
        preHandleFile();
    }

    m_symbol = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', 
        '&', '|', '<', '>', '=', '~'};

    m_keyword = {"class", "constructor", "function", "method", "field", "static",
        "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do",
        "if", "else", "while", "return"};
}

void JackTokenizer::preHandleFile() {
    std::string tmp_line("");
    while (std::getline(m_jack_file, tmp_line)) {
        
        tmp_line.erase(std::remove(tmp_line.begin(), tmp_line.end(), '\r'), tmp_line.end());
        tmp_line.erase(std::remove(tmp_line.begin(), tmp_line.end(), '\n'), tmp_line.end());

        if (tmp_line.size() == 0) {
            continue;
        }
 
        auto find_comments_pos = tmp_line.find("//");
        if (find_comments_pos == 0) {
            continue;
        }

        if (find_comments_pos != std::string::npos && find_comments_pos > 0) {
            tmp_line = tmp_line.substr(0, find_comments_pos);
        }
        
        //  去掉  /**,  *\/,  *, 
        find_comments_pos = tmp_line.find("/**");
        if (find_comments_pos == 0) {
            continue;
        }
        
        find_comments_pos = tmp_line.find(" *");
        if (find_comments_pos == 0) {
            continue;
        }
        
        find_comments_pos = tmp_line.find("*/");
        if (find_comments_pos != std::string::npos) {
            continue;
        }

        std::cout << "debug tmp_line:" << tmp_line << "\n";

        // 对所有行进行分解成 tokens.
        spliteTokens(tmp_line);
    }
}

void JackTokenizer::spliteTokens(const std::string& tmp_line) {
    std::string word;
    for (const auto& c : tmp_line) {
        if (c == ' ') { // 以空格为分隔符
            if (!word.empty()) { // 如果单词不为空，则添加到结果中
                m_tokens.push_back(word);
                word.clear();
            }
        } else if (m_symbol.find(c) != m_symbol.end()) {
            if (!word.empty()) { // 如果单词不为空，则添加到结果中
                m_tokens.push_back(word);
                word.clear();
            }  
            word.push_back(c);
            m_tokens.push_back(word);
            word.clear();
        } else {
            word += c; // 否则将字符添加到单词中
        }
    }

    if (!word.empty()) { // 处理最后一个单词
        m_tokens.push_back(word);
    }
    m_tokens_len = m_tokens.size();
}

JackTokenizer::~JackTokenizer() {
    if (m_jack_file.is_open()) {
        m_jack_file.close();
    }
}

bool JackTokenizer::hasMoreTokens() {
    return m_jack_file.is_open() && (m_pos < m_tokens_len);
}

void JackTokenizer::advance() {
    if (m_pos < m_tokens_len) {
        ++m_pos;
    }
}

bool JackTokenizer::checkDigit(const std::string& str_digit) {
    return !str_digit.empty() && std::find_if(str_digit.begin(), 
        str_digit.end(), [](unsigned char c) { return !std::isdigit(c); }) == str_digit.end();
}

bool JackTokenizer::checkString(const std::string& str_str) {
    // TODO 实现不太准确
    auto pos = str_str.find("\"");
    return pos == 0 ? true: false;
}
    
JackTokenizer::token_type JackTokenizer::tokenType() {
    const std::string& current_token = m_tokens.at(m_pos);
    if (m_keyword.find(current_token) != m_keyword.end()) {
        m_type = JackTokenizer::KEYWORD;
    } else if (current_token.size() == 1 && m_symbol.find(current_token[0]) != m_symbol.end()) {
        m_type = JackTokenizer::SYMBOL;
    } else if (checkDigit(current_token)) {
        m_type = JackTokenizer::INT_CONST;
    } else if (checkString(current_token)) {
        m_type = JackTokenizer::STRING_CONST; 
    } else {
        m_type = JackTokenizer::IDENTIFIER;
    }

    return m_type;
}

std::string JackTokenizer::keyword() {
    std::string keyword = m_tokens.at(m_pos);

    //std::transform(keyword.begin(), keyword.end(), keyword.begin(), [](char const &c) {
    //    return std::toupper(c);});
    
    return keyword;
}

std::string JackTokenizer::symbol() {
    return m_tokens.at(m_pos);
}

std::string JackTokenizer::identifier() {
    return m_tokens.at(m_pos);
}

int JackTokenizer::intVar() {
    return std::stoi(m_tokens.at(m_pos));
}

std::string JackTokenizer::stringVar() {
    std::string ret_str = m_tokens.at(m_pos);

    while(m_pos < m_tokens_len) {
        advance();

        std::string token = m_tokens.at(m_pos);
        ret_str += " " + token;

        if (token.find("\"") != std::string::npos) {
            break;
        }
    }

   return ret_str;
}






