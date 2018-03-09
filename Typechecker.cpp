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
    complexBoolExpr.first->accept(*this);
    auto type1 = currentType;
    complexBoolExpr.second->accept(*this);
    auto type2 = currentType;
    if(type1 != type2){
	throw TypecheckerException("Type Mismatch");
    }
    if(type1 == MPLType::BOOL){
    	if(complexBoolExpr.relation != Token::EQUAL && complexBoolExpr.relation != Token::NOT_EQUAL){
    	    throw TypecheckerException("Incorrect Boolean Evaluation");
    	}
    }
    if(complexBoolExpr.hasConjunction){
	complexBoolExpr.remainder->accept(*this);
    }
    currentType = MPLType::BOOL;
}

void Typechecker::visit(ASTStatementList& statementList) {
    table.pushTable();
    for (auto &statement : statementList.statements) {
        statement->accept(*this);
    }
    table.popTable();
}

void Typechecker::visit(ASTBasicIf& basicIf) {
    //basicIf.expression->accept(*this)
}

void Typechecker::visit(ASTIfStatement& ifStatement) {
    //how to traverse!
    ifStatement.baseIf.expression->accept(*this);
    ifStatement.baseIf.statementList->accept(*this);

    //else if's traverse
    int length = ifStatement.elseifs.size();
    for(int spot = 0; spot < length; spot++){
        ifStatement.elseifs[spot].expression->accept(*this);
        ifStatement.elseifs[spot].statementList->accept(*this);
    }
    //else
    if(ifStatement.elseList != nullptr){
        ifStatement.elseList->accept(*this);
    }
}

void Typechecker::visit(ASTWhileStatement& whileStatement) {
    // TODO
}

void Typechecker::visit(ASTPrintStatement& printStatement) {
    printStatement.expression->accept(*this);
}

void Typechecker::visit(ASTAssignmentStatement& assignmentStatement) {
    //ID in the table
    if(table.doesSymbolExist(assignmentStatement.identifier->name)){
        auto check_type = table.getSymbolType(assignmentStatement.identifier->name);
        assignmentStatement.rhs->accept(*this);

        if(check_type != currentType){
            throw TypecheckerException("Identifier '" + assignmentStatement.identifier->name + "' changed types after declared.");
        }


    }else{
        //ID not in the table
        assignmentStatement.rhs->accept(*this);
        if(currentType == MPLType::INT){
            table.storeInt(assignmentStatement.identifier->name);
        }
        else if(currentType == MPLType::STRING){
            table.storeString(assignmentStatement.identifier->name);
        }
        else if(currentType == MPLType::BOOL){
            table.storeBool(assignmentStatement.identifier->name);
        }
        else if(currentType == MPLType::ARRAY){
            table.storeVector(assignmentStatement.identifier->name);
        }
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
    complexExpression.firstOperand->accept(*this);
    auto type1 = currentType;
    complexExpression.rest->accept(*this);
    auto type2 = currentType;
    if(type1 != type2){
        throw TypecheckerException("Cannot do arithmetic on different types.");
    }
    //assumption that type1 and type2 are the same
    if(type1 == MPLType::STRING){
        if(complexExpression.operation != Token::PLUS){
            throw TypecheckerException("Invalid operation on a string: " + toString(type1));
        }
    }

    if(type1 == MPLType::BOOL){
        throw TypecheckerException("Invalid operation on a boolean.");
    }
    //for assign to be able to deal with
    currentType = type1;
}
