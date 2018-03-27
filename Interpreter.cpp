#include <iostream>
#include <sstream>

#include "Interpreter.h"
#include "ASTNodes.h"

using namespace std;

void Interpreter::visit(ASTSimpleBoolExpression& simpleBoolExpr) {
    simpleBoolExpr.expression->accept(*this);
    // Note: after visiting the above expression, (assuming it's boolean)
    // it will set currentBool to be whatever boolean value it represents
    if (currentType != MPLType::BOOL) {
        throw InterpreterException("Expected boolean expression");
    }
    if (simpleBoolExpr.negated) {
        currentBool = !currentBool;
    }
    currentType = MPLType::BOOL;
}

void Interpreter::visit(ASTComplexBoolExpression& complexBoolExpr) {
    complexBoolExpr.first->accept(*this);
    auto lhsType = currentType;
    auto lhsInt = currentInt;
    auto lhsString = currentString;
    auto lhsBool = currentBool;
    complexBoolExpr.second->accept(*this);
    auto rhsType = currentType;
    auto rhsInt = currentInt;
    auto rhsString = currentString;
    auto rhsBool = currentBool;
    bool myResult;

    // DONE: figure out what comparison to make, do that comparison,
    // and store the result in myResult.

    // Check to see if types are equal
    if(lhsType != rhsType){
	throw InterpreterException("Type Mismatch");
    }
    // Check to see if trying to compare arrays
    if(lhsType == MPLType::ARRAY){
        throw InterpreterException("Cannot Compare Arrays");
    }
    // Switch based on the comparison, within each case is another switch based on type.
    // If a bad type is used, an exception is thrown. If all is well, assign myResult.
    switch(complexBoolExpr.relation){
        case Token::EQUAL:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt == rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString == rhsString;
                    break;
                case MPLType::BOOL:
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}
            break;
        case Token::NOT_EQUAL:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt != rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString != rhsString;
                    break;
                case MPLType::BOOL:
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}            
			break;
        case Token::LESS_THAN:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt < rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString < rhsString;
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}            
			break;
        case Token::LESS_THAN_EQUAL:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt <= rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString <= rhsString;
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}            
			break;
        case Token::GREATER_THAN:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt > rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString > rhsString;
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}            
			break;
        case Token::GREATER_THAN_EQUAL:
            switch(lhsType){
                case MPLType::INT:
                    myResult = lhsInt >= rhsInt;
                    break;
                case MPLType::STRING:
                    myResult = lhsString >= rhsString;
                    break;
                default:
                    throw InterpreterException("Bad Type Error");
			}            
			break;
	}
    if (complexBoolExpr.hasConjunction) {
        complexBoolExpr.remainder->accept(*this);
        if (complexBoolExpr.conjunction == Token::AND) {
            currentBool = myResult && currentBool;
        } else {
            currentBool = myResult || currentBool;
        }
    } else {
        currentBool = myResult;
    }
    if (complexBoolExpr.negated) {
        currentBool = !currentBool;
    }
    currentType = MPLType::BOOL;
}

void Interpreter::visit(ASTStatementList& statementList) {
    table.pushTable();
    for (auto &statement : statementList.statements) {
        statement->accept(*this);
    }
    table.popTable();
}

void Interpreter::visit(ASTBasicIf& basicIf) {
    // I strongly suggest not using this function and instead having
    // visit(ASTIfStatement) explicitly look inside the various baseIfs.
    // This is because once you enter a certain if statementList, you will
    // not want to enter any other statementLists.
    abort();
}

void Interpreter::visit(ASTIfStatement& ifStatement) {
    ifStatement.baseIf.expression->accept(*this);
	if(currentBool){
    	ifStatement.baseIf.statementList->accept(*this);
		return;
	}
	int length = ifStatement.elseifs.size();
	for(int spot = 0; spot < length; spot++){
		ifStatement.elseifs[spot].expression->accept(*this);
		if(currentBool){
    		ifStatement.elseifs[spot].statementList->accept(*this);
			return;
		}
	}
	if(ifStatement.elseList != nullptr){
		ifStatement.elseList->accept(*this);
		return;
	}
}

void Interpreter::visit(ASTWhileStatement& whileStatement) {
    whileStatement.condition->accept(*this);
    while (currentBool) {
        whileStatement.statements->accept(*this);
        whileStatement.condition->accept(*this);
    }
}

void Interpreter::visit(ASTPrintStatement& printStatement) {
	// TODO    
	printStatement.expression->accept(*this);
	if(currentType == MPLType::INT){
        cout << currentInt;
    }else if(currentType == MPLType::BOOL){
        cout << currentBool;
    }else if(currentType == MPLType::STRING){
        cout << currentString;
    }else if(currentType == MPLType::ARRAY){
        // table.storeVector(assignmentStatement.identifier-> name, CURRENTVECTOR);
    }

    if(printStatement.isPrintln){
        cout << "\n";
    }
}

void Interpreter::visit(ASTAssignmentStatement& assignmentStatement) {
    // TODO
    assignmentStatement.rhs->accept(*this);
    if(currentType == MPLType::INT){
        table.storeIntVal(assignmentStatement.identifier->name, currentInt);
    }else if(currentType == MPLType::BOOL){
        table.storeBoolVal(assignmentStatement.identifier->name, currentBool);
    }else if(currentType == MPLType::STRING){
        table.storeStringVal(assignmentStatement.identifier->name, currentString);
    }else if(currentType == MPLType::ARRAY){
        // table.storeVector(assignmentStatement.identifier-> name, CURRENTVECTOR);
    }
}

void Interpreter::visit(ASTIdentifier& identifier) {
    currentType = table.getSymbolType(identifier.name);
    if(currentType == MPLType::INT){
	currentInt = table.getIntVal(identifier.name);
    }else if(currentType == MPLType::BOOL){
        currentBool = table.getBoolVal(identifier.name);
    }else if(currentType == MPLType::STRING){
        currentString = table.getStringVal(identifier.name);
    }else if(currentType == MPLType::ARRAY){
        // DO THE ARRAY THINGS
    }
}

void Interpreter::visit(ASTLiteral& literal) {
    currentType = literal.type;
    if (currentType == MPLType::INT) {
        // istringstream allows us to create something like "cin" but instead
        // of reading from the keyboard, it reads from the string that we give
        // it
        istringstream iss(literal.value);
        iss >> currentInt;
        // If there was an error reading the integer out of the istringstream,
        // it will evaluate to false when used in a boolean context.
        if (!iss) {
            throw InterpreterException("Bad integer literal: " + literal.value);
        }
    } else if (currentType == MPLType::BOOL) {
        // also could be written as
        // currentBool = literal.value == "true", but that looks confusing
        if (literal.value == "true") {
            currentBool = true;
        } else {
            currentBool = false;
        }
    } else {
        currentString = literal.value;
    }
}

void Interpreter::visit(ASTListLiteral& listLiteral) {
    // TODO? (only needed for extra credit)
}

void Interpreter::visit(ASTReadExpression& readExpression) {
    cout << readExpression.message;
    if (readExpression.isReadInt) {
        cin >> currentInt;
        currentType = MPLType::INT;
    } else {
        // getline will read everything up until a newline character, allowing
        // us to read in a string containing spaces
        getline(cin, currentString);
        currentType = MPLType::STRING;
    }
    // If there was an input problem (say, a user entering 'test' to readint),
    // cin will be false when used in a boolean context.  This way, if the
    // user enters something silly, we can throw an error.
    if (!cin) {
        throw InterpreterException("Invalid user input");
    }
    // If we read in a number, ignore everything else on the current line
    if (readExpression.isReadInt) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void Interpreter::visit(ASTComplexExpression& complexExpression) {
    complexExpression.firstOperand->accept(*this);
	auto firstType = currentType;
	// Default values will be changed before use.	
	auto firstInt = 0;
	std::string firstString = "";
	auto firstBool = false;
	if(currentType == MPLType::INT){
		firstInt = currentInt;
	}
	else if(currentType == MPLType::STRING){
		firstString = currentString;
	}
	else if(currentType == MPLType::BOOL){
		firstBool = currentBool;
	}
	complexExpression.rest->accept(*this);
	if(complexExpression.operation == Token::PLUS){
		if(currentType == MPLType::INT){
			currentInt = firstInt + currentInt;
		}else if(currentType == MPLType::STRING){
			currentString = firstString + currentString;
		}else {
			throw InterpreterException("Operation Mismatch");
		}
	}else if(complexExpression.operation == Token::MINUS){
		if(currentType == MPLType::INT){
			currentInt = firstInt - currentInt;
		}else{
			throw InterpreterException("Operation Mismatch");
		}
	}else if(complexExpression.operation == Token::MULTIPLY){
		if(currentType == MPLType::INT){
			currentInt = firstInt * currentInt;
		}else{
			throw InterpreterException("Operation Mismatch");
		}
	}else if(complexExpression.operation == Token::DIVIDE){
		if(currentType == MPLType::INT){
			currentInt = firstInt / currentInt;
		}else {
			throw InterpreterException("Operation Mismatch");
		}
	}else if(complexExpression.operation == Token::MODULUS){
		if(currentType == MPLType::INT){
			currentInt = firstInt % currentInt;
		}else if(currentType == MPLType::STRING){
			throw InterpreterException("Do you even know how to use strings?");
		}else {
			throw InterpreterException("Operation Mismatch");
		}
	}
}

