#include <iostream>

#include "SymbolTable.h"

using namespace std;

Symbol::Symbol() {
    type = MPLType::INT;
}
MPLType Symbol::getType() {
    return type;
}

void Symbol::setInt() {
    type = MPLType::INT;
}
void Symbol::setString() {
    type = MPLType::STRING;
}
void Symbol::setBool() {
    type = MPLType::BOOL;
}
void Symbol::setVector() {
    type = MPLType::ARRAY;
}

void SymbolTable::pushTable() {
    scopes.push_back(map<string, Symbol>());
}

void SymbolTable::popTable() {
    if (scopes.size() == 0) {
        throw SymbolTableException("Internal error: attempting to pop final table");
    }
    scopes.pop_back();
}

void SymbolTable::storeInt(std::string name) {
    auto *info = getSymbol(name);
    if (info && info->getType() != MPLType::INT) {
        throw SymbolTableException("Variable " + name + " is not of type int");
    }
    if (!info) {
        info = createSymbol(name);
    }
    info->setInt();
}

void SymbolTable::storeString(std::string name) {
    auto *info = getSymbol(name);
    if (info && info->getType() != MPLType::STRING) {
        throw SymbolTableException("Variable " + name + " is not of type string");
    }
    if (!info) {
        info = createSymbol(name);
    }
    info->setString();
}

void SymbolTable::storeBool(std::string name) {
    auto *info = getSymbol(name);
    if (info && info->getType() != MPLType::BOOL) {
        throw SymbolTableException("Variable " + name + " is not of type bool");
    }
    if (!info) {
        info = createSymbol(name);
    }
    info->setBool();
}

void SymbolTable::storeVector(std::string name) {
    auto *info = getSymbol(name);
    if (info && info->getType() != MPLType::ARRAY) {
        throw SymbolTableException("Variable " + name + " is not of type array");
    }
    if (!info) {
        info = createSymbol(name);
    }
    info->setVector();
}

bool SymbolTable::doesSymbolExist(std::string name) {
    return getSymbol(name);
}

MPLType SymbolTable::getSymbolType(std::string name) {
    auto *info = getSymbol(name);
    if (!info) {
        throw SymbolTableException("Internal error: Unable to find symbol " + name);
    }
    return info->getType();
}

// Returns a pointer to the specified symbol, or nullptr if the symbol
// doesn't exist.
Symbol *SymbolTable::getSymbol(std::string name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto itSymbol = it->find(name);
        if (itSymbol != it->end()) {
            return &itSymbol->second;
        }
    }
    
    return nullptr;
}

// Create a new symbol and return a pointer to it.
Symbol *SymbolTable::createSymbol(std::string name) {
    auto ans = scopes.back().insert(make_pair(name, Symbol()));
    return &ans.first->second;
}
