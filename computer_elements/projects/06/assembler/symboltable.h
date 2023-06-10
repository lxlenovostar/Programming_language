#ifndef SYMBLTABLE 
#define SYMBLTABLE 

#include <string>
#include <map>

class Symboltable
{
public:
    Symboltable() {
        m_symboltable["sp"] = "0";
        m_symboltable["LCL"] = "1";
        m_symboltable["ARG"] = "2";
        m_symboltable["THIS"] = "3";
        m_symboltable["THAT"] = "4";
        m_symboltable["SCREEN"] = "16384";
        m_symboltable["KBD"] = "24576";
        
        m_symboltable["R0"] = "0";
        m_symboltable["R1"] = "1";
        m_symboltable["R2"] = "2";
        m_symboltable["R3"] = "3";
        m_symboltable["R4"] = "4";
        m_symboltable["R5"] = "5";
        m_symboltable["R6"] = "6";
        m_symboltable["R7"] = "7";
        m_symboltable["R8"] = "8";
        m_symboltable["R9"] = "9";
        m_symboltable["R10"] = "10";
        m_symboltable["R11"] = "11";
        m_symboltable["R12"] = "12";
        m_symboltable["R13"] = "13";
        m_symboltable["R14"] = "14";
        m_symboltable["R15"] = "15";
    }

    virtual ~Symboltable();
    void addEntry(const std::string& symbol, int address);
    bool contains(const std::string& symbol);
    int GetAddress(const std::string& symbol);

private:
    std::map<std::string, std::string> m_symboltable;
};

#endif 