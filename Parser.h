#ifndef PARSER_H
#define PARSER_H

#include <string>

#include "MyPLException.h"
#include "Lexer.h"
#include "ASTNodes.h"

class ParserException: public MyPLException {
public:
    ParserException(std::string msg = ""): MyPLException(msg) {}
};

class Parser {
public:
    Parser(Lexer &lex);

    std::shared_ptr<ASTNode> getAST();
private:
    void advance();

    void eat(Token token, std::string message);

    [[noreturn]] void error(std::string message);

    //the organization of this is top-level to the bottom-levelish
    std::shared_ptr<ASTStatementList> stmts(std::shared_ptr<ASTStatementList> lst = nullptr);
    std::shared_ptr<ASTStatement> stmt();

    //outputting to the user
    std::shared_ptr<ASTPrintStatement> output();

    //getting input from the user
    std::shared_ptr<ASTReadExpression> input();
    //assigning a variable some value
    std::shared_ptr<ASTAssignmentStatement> assign();
    //checking for a list format
    //this is used with the assign and the value functions
    std::shared_ptr<ASTExpression> listindex();

    //represented a generic expression of any sort of thing done to a variable or
    //evaluating the variable
    std::shared_ptr<ASTExpression> expr();

    //the tail of expr, which is above this
    void exprt(std::shared_ptr<ASTComplexExpression> expression);

    //deciphering the contents of a value
    //this can be a multitude of things, as referenced by the language scope
    std::shared_ptr<ASTExpression> value();
    //checking for a list format
    std::shared_ptr<ASTListLiteral> exprlist();
    //the tail of the function above, which is exprlist
    void exprtail(std::shared_ptr<ASTListLiteral> list);

    // for a while loop
    std::shared_ptr<ASTWhileStatement> loop();

    // essentially an if statement
    std::shared_ptr<ASTIfStatement> cond();
    //the tail of the condition

    void condt(std::shared_ptr<ASTIfStatement> statement);
    //boolean expression evaluation
    std::shared_ptr<ASTBoolExpression> bexpr();
    //the tail of the bexpr
    void bexprt(std::shared_ptr<ASTComplexBoolExpression> expression);
    //evaluating for relation operators
    void bconnect(std::shared_ptr<ASTComplexBoolExpression> expression);
    //math operators such as +,-,*,%,/
    void math_rel(std::shared_ptr<ASTComplexExpression> expression);

    //all of these are checks for either a literal value
    //or for the first lexeme to line up to the other ones.
    bool is_math_rel();
    bool is_bconnect();
    bool is_value();
    bool is_exprt();
    bool is_stmt();
    bool is_input();
    bool is_bool_rel();
    bool is_list_index();



    Lexer &lexer;

    Lexeme currentLexeme;

    std::shared_ptr<ASTNode> syntaxTree;
};

#endif /* PARSER_H */
