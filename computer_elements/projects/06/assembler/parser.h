#ifndef PARSER
#define PARSER

#include <string>
#include <fstream>
#include <vector>

class Parser {
    public:
        enum command_type {
            A_COMMAND = 0, 
            C_COMMAND,
            L_COMMAND, 
            NULL_COMMAND
        };

        Parser(const std::string& filename);
        void advance(); 
        virtual ~Parser();
        bool hasMoreCommands();
        command_type commandType();
        std::string symbol();
        std::string dest();
        std::string comp();
        std::string jump();

        inline int get_pos() { return m_pos; }

    private:
        int m_pos;
        std::string m_curr_line;
        std::ifstream m_assembler_file;
        // TODO 以下三个变量换成 结构体
        std::string m_curr_dest;
        std::string m_curr_comp;
        std::string m_curr_jump;
        command_type m_type;
        std::vector<std::string> m_command; 

        void handleCCommand();
        void preHandleFile();
};

#endif