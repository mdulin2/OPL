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
    if (type1 == MPLType::ARRAY || type2 == MPLType::ARRAY){
	      throw TypecheckerException("Cannot compare lists together");
    }
    //checks the types of the two compared expressions
    if(type1 != type2){
	       throw TypecheckerException("Type Mismatch");
    }

    //only the boolean can't have the other equality measures
    if(type1 == MPLType::BOOL){
    	if(complexBoolExpr.relation != Token::EQUAL && complexBoolExpr.relation != Token::NOT_EQUAL){
    	    throw TypecheckerException("Incorrect Boolean Evaluation");
    	}
    }
    //conjection
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
    //just ended up not wanting to use this...
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
    whileStatement.condition->accept(*this);
    whileStatement.statements->accept(*this);
}

void Typechecker::visit(ASTPrintStatement& printStatement) {
    printStatement.expression->accept(*this);
    if(currentType == MPLType::ARRAY){
        throw TypecheckerException("Lists cannot be printed.");
    }
}

void Typechecker::visit(ASTAssignmentStatement& assignmentStatement) {
    //ID in the table
    if(table.doesSymbolExist(assignmentStatement.identifier->name)){

        auto check_type = table.getSymbolType(assignmentStatement.identifier->name);
        assignmentStatement.rhs->accept(*this);

        //checks if the value has an index and should not
        if(assignmentStatement.identifier->indexExpression != nullptr){
            if(currentType != MPLType::ARRAY){
                throw TypecheckerException("Identifier '" + assignmentStatement.identifier->name + "' has an Invalid index");
            }
        }

        //checking if the types are the same or not
        if(check_type != currentType){
            throw TypecheckerException("Identifier '" + assignmentStatement.identifier->name + "' changed types after declared.");
        }
        //checks to see if the types in the array are the same as previously.
        if(currentType == MPLType::ARRAY){
            auto new_type = table.getSymbolTypeArray(assignmentStatement.identifier->name);
            if(new_type != array_type){
                throw TypecheckerException("Arrays don't have the same type in them.");
            }
        }

    }else{
        //ID not in the table
        assignmentStatement.rhs->accept(*this);
        //checks if the value has an index and should not
        if(assignmentStatement.identifier->indexExpression != nullptr){
            if(currentType != MPLType::ARRAY){
                throw TypecheckerException("Identifier '" + assignmentStatement.identifier->name + "' has an Invalid index");
            }
        }
        //assigning initial types
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

            if(assignmentStatement.identifier->indexExpression != nullptr){

                //checks the index being used
                assignmentStatement.identifier->indexExpression->accept(*this);
                //the index is an integer
                if(currentType != MPLType::INT){
                    throw TypecheckerException("Identifier '" + assignmentStatement.identifier->name + "' has an Invalid index");
                }
            }
            //array_type is the type in the array
            table.storeVector(assignmentStatement.identifier->name,array_type);
        }
        else{
            throw TypecheckerException("Not a valid type.");
        }
    }
}

void Typechecker::visit(ASTIdentifier& identifier) {
    if (table.doesSymbolExist(identifier.name)) {
        currentType = table.getSymbolType(identifier.name);
    } else {
        throw TypecheckerException("Identifier " + identifier.name + " used before defined");
    }
    //checks for indexs
    if (identifier.indexExpression && currentType != MPLType::ARRAY) {
        throw TypecheckerException("Identifier " + identifier.name + " given an index when not an array");
    }
}

void Typechecker::visit(ASTLiteral& literal) {
    currentType = literal.type;
}

void Typechecker::visit(ASTListLiteral& listLiteral) {

    if(listLiteral.expressions.size() == 0){
        return;
    }

    listLiteral.expressions[0]->accept(*this);
    array_type = currentType;
    for(int i = 1; i < listLiteral.expressions.size(); i++ ){
        listLiteral.expressions[i]->accept(*this);

        //makes sure that the two types in the list are the same
        if(array_type != currentType){
            throw TypecheckerException("Array items not of the same type!" );
        }
    }
    currentType = MPLType::ARRAY;
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
    auto array1 = array_type;
    complexExpression.rest->accept(*this);
    auto array2 = array_type;
    auto type2 = currentType;
    if(type1 != type2){
        throw TypecheckerException("Cannot do arithmetic on different types.");
    }

    if(type1 == MPLType::ARRAY){
        if(array1 != array2){
            throw TypecheckerException("Array types are not the same. Cannot be added together");
        }
        if(complexExpression.operation != Token::PLUS){
            throw TypecheckerException("Invalid operation on array " + toString(type1));
        }
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
