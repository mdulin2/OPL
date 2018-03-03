#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include <map>
#include <string>

#include "MyPLException.h"
#include "common.h"

class SymbolTableException: public MyPLException {
public:
    SymbolTableException(std::string msg=""): MyPLException(msg) {}
};

// Represents a symbol in the symbol table.  This class will eventually
// also store a value associated with it.
class Symbol {
public:
    Symbol();
    
    // Return current symbol type
    MPLType getType();
    
    // Set symbol to be of type int
    void setInt();
    
    // Set symbol to be of type string
    void setString();
    
    // Set symbol to be of type bool
    void setBool();
    
    // Set symbol to be of type array
    void setVector();
private:
    MPLType type;
};

// Represents a symbol table, with support for nested scopes.  When a new
// symbol is created, it will be added to the innermost scope.  When a variable
// is updated, it will be updated in whatever scope it already exists in.
class SymbolTable {
public:
    // Move into another scope
    void pushTable();
    
    // Return from a scope
    void popTable();
    
    // Store that the given variable is of type int.  Throws an exception
    // if the variable is already of another type.
    void storeInt(std::string name);
    
    // Store that the given variable is of type string.  Throws an exception
    // if the variable is already of another type.
    void storeString(std::string name);
    
    // Store that the given variable is of type bool.  Throws an exception
    // if the variable is already of another type.
    void storeBool(std::string name);
    
    // Store that the given variable is of type array.  Throws an exception
    // if the variable is already of another type.
    void storeVector(std::string name);
    
    // Returns whether the specified symbol exists in any current scopes
    bool doesSymbolExist(std::string name);
    
    // Returns the type of the specified symbol.  Throws an exception if the
    // symbol doesn't exist.
    MPLType getSymbolType(std::string name);
private:
    Symbol *getSymbol(std::string name);
    Symbol *createSymbol(std::string name);
    
    std::vector<std::map<std::string, Symbol>> scopes;
};

#endif /* SYMBOLTABLE_H */
