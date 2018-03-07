#include "Typechecker.h"
#include "ASTNodes.h"
#include <iostream>
#include <string>
using namespace std;

void Typechecker::visit(ASTSimpleBoolExpression& simpleBoolExpr) {
    simpleBoolExpr.expression->accept(*this);
    if (currentType != MPLType::BOOL) {
        throw TypecheckerException("Expected boolean expression");
    }
    currentType = MPLType::BOOL;
}

void Typechecker::visit(ASTComplexBoolExpression& complexBoolExpr) {
    currentType = MPLType::BOOL;
    cout << "yhrough complex?" << endl;
}

void Typechecker::visit(ASTStatementList& statementList) {
    table.pushTable();
    for (auto &statement : statementList.statements) {
        statement->accept(*this);
    }
    table.popTable();

}

void Typechecker::visit(ASTBasicIf& basicIf) {
    //basicIf.expression->accept(*this);
    cout << "asdfasdf" << endl;
}

void Typechecker::visit(ASTIfStatement& ifStatement) {
    //how to traverse!
    ifStatement.baseIf.expression->accept(*this);
    ifStatement.baseIf.statementList->accept(*this);

    //else if's traverse
    int length = ifStatement.elseifs.size();
    cout << "length: " << length << endl;
    for(int spot = 0; spot < length; spot++){

    }

    
    //else
}

void Typechecker::visit(ASTWhileStatement& whileStatement) {
    // TODO
}

void Typechecker::visit(ASTPrintStatement& printStatement) {
    printStatement.expression->accept(*this);
}

void Typechecker::visit(ASTAssignmentStatement& assignmentStatement) {
    if(table.doesSymbolExist(assignmentStatement.identifier->name)){
        //how to know what kind of Variable to push?
    }else{
        cout << "Insert into the table" << endl;
    }

}

void Typechecker::visit(ASTIdentifier& identifier) {
    if (table.doesSymbolExist(identifier.name)) {
        currentType = table.getSymbolType(identifier.name);
    } else {
        throw TypecheckerException("Identifier " + identifier.name + " used before defined");
    }
    if (identifier.indexExpression && currentType != MPLType::ARRAY) {
        throw TypecheckerException("Identifier " + identifier.name + " given an index when not an array");
    }
}

void Typechecker::visit(ASTLiteral& literal) {
    currentType = literal.type;
}

void Typechecker::visit(ASTListLiteral& listLiteral) {
    // TODO
}

void Typechecker::visit(ASTReadExpression& readExpression) {
    if (readExpression.isReadInt) {
        currentType = MPLType::INT;
    } else {
        currentType = MPLType::STRING;
    }
}

//types must be the same in the comparison
//relational operator needs to be possible for that type
//can't do greater than less than for a boolean
void Typechecker::visit(ASTComplexExpression& complexExpression) {

}

int Typechecker::getType(ASTLiteral& literal){
    //TODO
}
