#ifndef JACKTOKENIZER
#define JACKTOKENIZER

#include <string>
#include <fstream>
#include <vector>
#include <set>

class JackTokenizer {
    public:
        enum token_type {
            KEYWORD = 0, 
            SYMBOL,
            IDENTIFIER,
            INT_CONST,
            STRING_CONST,
            NULL_TYPE
        };

        enum keyword_type {
            K_CLASS = 0,
            K_METHOD,
            K_INT, 
            K_FUNCTION,
            K_BOOL,
            K_CONSTRUCTOR,
            K_CHAR,
            K_VOID,
            K_VAR,
            K_STATIC,
            K_FIELD,
            K_LET, 
            K_DO,
            K_IF,
            K_ELSE,
            K_WHILE,
            K_RETURN,
            K_TRUE,
            K_FALSE,
            K_NULL,
            K_THIS,
            K_NULL_KEYWORD
        };

        JackTokenizer(const std::string& filename);
        virtual ~JackTokenizer();
        bool hasMoreTokens();
        void advance(); 
        token_type tokenType();
        //keyword_type keyword();
        std::string keyword();
        std::string symbol();
        std::string identifier();
        int intVar();        
        std::string stringVar();

    private:
        int m_pos;
        std::ifstream m_jack_file;
        token_type m_type;
        std::vector<std::string> m_tokens; 
        int m_tokens_len;
        std::set<char> m_symbol;
        std::set<std::string> m_keyword;

        void preHandleFile();
        void spliteTokens(const std::string& tmp_line);
        bool checkDigit(const std::string& str_digit); 
        bool checkString(const std::string& str_str); 
};

#endif