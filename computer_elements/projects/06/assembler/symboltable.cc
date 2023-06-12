#include <string>
#include <map>
#include <iostream>

#include "symboltable.h"

Symboltable:: ~Symboltable() {
}

void Symboltable::addEntry(const std::string& symbol, int address) {
    m_symboltable[symbol] = std::to_string(address);
}
    
bool Symboltable::contains(const std::string& symbol) {
    return m_symboltable.find(symbol) != m_symboltable.end();
}

std::string Symboltable::GetAddress(const std::string& symbol) {
    return m_symboltable.at(symbol);
}