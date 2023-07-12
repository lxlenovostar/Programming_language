#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "parser.h"
#include "codewriter.h"

int main(int argc, char** argv) {
    const std::string& infilename(argv[1]);
    const std::string& outfilename(argv[2]);
    std::shared_ptr<Parser> parser = std::make_shared<Parser>(infilename);
    std::shared_ptr<CodeWriter> codewriter = std::make_shared<CodeWriter>(outfilename);
  
    while (parser->hasMoreCommands()) {
        parser->advance();

        std::cout << "debug: " << parser->getCurrLine() << std::endl;

        Parser::command_type ret_type = parser->commandType();
        if (ret_type == Parser::C_PUSH) {
            codewriter->writePushPop("push", parser->arg1(), parser->arg2());
        } else if (ret_type == Parser::C_POP) {
            codewriter->writePushPop("pop", parser->arg1(), parser->arg2());
        } else if (ret_type == Parser::C_ARITHMETIC) {
            codewriter->writeArithmetic(parser->arg1());
        } else if (ret_type == Parser::C_LABEL) {
            codewriter->writeLabel(parser->arg1());
        } else if (ret_type == Parser::C_IF) {
            codewriter->WriteIf(parser->arg1());
        } else if (ret_type == Parser::C_GOTO) {
            codewriter->WriteGoto(parser->arg1());
        } else if (ret_type == Parser::C_FUNCTION) {
            codewriter->WriteFunction(parser->arg1(), parser->arg2());
        } 

    }

    return 0;
}