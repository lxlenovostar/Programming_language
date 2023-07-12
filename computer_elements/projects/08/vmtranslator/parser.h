#ifndef PARSER
#define PARSER

#include <string>
#include <fstream>
#include <vector>

class Parser {
    public:
        enum command_type {
            C_ARITHMETIC = 0, 
            C_PUSH,
            C_POP,
            C_LABEL,
            C_GOTO,
            C_IF,
            C_FUNCTION,
            C_CALL,
            C_RETURN,
            NULL_COMMAND
        };

        Parser(const std::string& filename);
        void advance(); 
        virtual ~Parser();
        bool hasMoreCommands();
        command_type commandType();
        std::string arg1();
        int arg2();
        /* for debug */
        std::string getCurrLine() { return m_curr_line; }

    private:
        int m_pos;
        std::string m_curr_line;
        std::ifstream m_assembler_file;
        command_type m_type;
        std::vector<std::string> m_command; 
        std::vector<std::string> m_curr_word;

        void preHandleFile();
};

#endif