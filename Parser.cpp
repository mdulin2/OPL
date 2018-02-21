#include "Parser.h"
#include <iostream>
#include <sstream>
using namespace std;

const bool USE_CONTEXTLOG = false;

class ContextLog {
public:
    ContextLog(string p, const Lexeme &lex):
    part(p),
    l(lex.line),
    c(lex.col) {
        if (USE_CONTEXTLOG)
            cout << "Entering " << part << "(" << l << ":" << c << ")" << endl;
    }
    ~ContextLog() {
        if (USE_CONTEXTLOG)
            cout << "Exiting " << part << "(" << l << ":" << c << ")" << endl;
    }

private:
    string part;
    int l;
    int c;
};

Parser::Parser(Lexer &lex): lexer(lex) {
    advance();
    syntaxTree = stmts();
}

shared_ptr<ASTNode> Parser::getAST() {
    return syntaxTree;
}

void Parser::advance() {
    currentLexeme = lexer.next();
}

void Parser::eat(Token token, std::string message) {
    if (currentLexeme.token == token) {
        advance();
    } else {
        error(message);
    }
}

void Parser::error(std::string message) {
    cout << "Error found" << endl;
    ostringstream oss;
    oss << message << ", found " << currentLexeme << " instead";
    throw ParserException(oss.str());
}

std::shared_ptr<ASTStatementList> Parser::stmts(std::shared_ptr<ASTStatementList> lst) {
    ContextLog clog("stmts", currentLexeme);
    if (!lst) lst = make_shared<ASTStatementList>();
    switch (currentLexeme.token) {
        case Token::PRINT: //print
        case Token::PRINTLN:
        case Token::ID: //assign
        case Token::IF: //if statement
        case Token::WHILE: //loop
            lst->statements.push_back(stmt());
            stmts(lst);
            break;
        default:
            // May be empty
            break;
    }
    return lst;
}

std::shared_ptr<ASTStatement> Parser::stmt() {
    ContextLog clog("stmt", currentLexeme);
    switch (currentLexeme.token) {
        case Token::PRINT:
        case Token::PRINTLN:
            return output();
            break;
        case Token::ID:
            return assign();
            break;
        case Token::IF:
            return cond();
            break;
        case Token::WHILE:
            return loop();
            break;
        default:
            error("Expected statement");
    }
}

std::shared_ptr<ASTPrintStatement> Parser::output() {
    ContextLog clog("output", currentLexeme);
    auto ans = std::make_shared<ASTPrintStatement>();
    if (currentLexeme.token == Token::PRINT) {
        eat(Token::PRINT, "Expected print");
        ans->isPrintln = false;
    } else {
        eat(Token::PRINTLN, "Expected print or println");
        ans->isPrintln = true;
    }
    eat(Token::LPAREN, "Expected '('");
    ans->expression = expr();
    eat(Token::RPAREN, "Expected ')'");
    eat(Token::SEMICOLON, "Expected ';'");
    return ans;
}

std::shared_ptr<ASTReadExpression> Parser::input() {
    ContextLog clog("input", currentLexeme);
    auto ans = std::make_shared<ASTReadExpression>();
    if(currentLexeme.token == Token::READINT){
        ans->isReadInt = true;
    }
    else if(currentLexeme.token == Token::READSTR){
        ans->isReadInt = false;
    }
    else{
        error("Expected a READSTR or READINT");
    }
    advance();

    eat(Token::LPAREN,"Expected a LPAREN");
    ans->message = currentLexeme.text;
    eat(Token::STRING,"Expected a STRING");
    eat(Token::RPAREN,"Expected a RPAREN");
    return ans;
}

std::shared_ptr<ASTAssignmentStatement> Parser::assign() {
    ContextLog clog("assign", currentLexeme);
    auto ans = make_shared<ASTAssignmentStatement>();

    //check for list assign...
    ans->identifier = make_shared<ASTIdentifier>();
    ans->identifier->name = currentLexeme.text;
    advance();
    // check to see if list index
    eat(Token::ASSIGN,"expected assignment");
    ans->rhs = expr();
    eat(Token::SEMICOLON,"expected semicolon");
    return ans;
}

std::shared_ptr<ASTExpression> Parser::listindex() {
    ContextLog clog("listindex", currentLexeme);
    auto ans = make_shared<ASTAssignmentStatement>();
    // TODO
    return nullptr;
}

std::shared_ptr<ASTExpression> Parser::expr() {
    ContextLog clog("expr", currentLexeme);

    //shared_ptr(<ASTExpression>)
    auto ans = value();

    if(is_math_rel()){
        auto complex = make_shared<ASTComplexExpression>();
        complex->firstOperand = ans;
        exprt(complex);
        return complex;
    }
    else{
        return ans;
    }

}

void Parser::exprt(std::shared_ptr<ASTComplexExpression> expression) {
    ContextLog clog("exprt", currentLexeme);
    math_rel(expression);
    expression->rest = expr();
    return;
}

void Parser::math_rel(std::shared_ptr<ASTComplexExpression> expression){
    ContextLog clog("exprt", currentLexeme);
    if(currentLexeme.token == Token::PLUS){
    }else if(currentLexeme.token == Token::MINUS){
    }else if(currentLexeme.token == Token::DIVIDE){
    }else if(currentLexeme.token == Token::MULTIPLY){
    }else if(currentLexeme.token == Token::MODULUS){
    }else if(currentLexeme.token == Token::EQUAL){
    }else{
        error("Invalid evaluator");
    }
    expression->operation = currentLexeme.token;
    advance();
    return;
}

bool isTokenAValue(Token token) {
    switch (token) {
        case Token::ID:
        case Token::STRING:
        case Token::INT:
        case Token::BOOL:
        case Token::READINT:
        case Token::READSTR:
        case Token::LBRACKET:
            return true;
            break;
        default:
            return false;
    }
}

std::shared_ptr<ASTExpression> Parser::value() {
    ContextLog clog("value", currentLexeme);
     if(currentLexeme.token == Token::ID){
        auto ans = make_shared<ASTIdentifier>();
        ans->name = currentLexeme.text;
        //the varible needs to be checked for lists indexing somewhere...

        eat(Token::ID,"Expected an ID");
        return ans;
    }else if(currentLexeme.token == Token::STRING){
        auto ans = make_shared<ASTLiteral>();
        ans->type = MPLType::STRING;
        ans->value = currentLexeme.text;
        advance();
        return ans;
    }else if(currentLexeme.token == Token::INT){
        auto ans = make_shared<ASTLiteral>();
        ans->type = MPLType::INT;
        ans->value = currentLexeme.text;
        advance();
        return ans;
    }else if(currentLexeme.token == Token::BOOL){
        auto ans = make_shared<ASTLiteral>();
        ans->type = MPLType::BOOL;
        ans->value = currentLexeme.text;
        advance();
        return ans;

    }else if(is_input()){
        auto ans = make_shared<ASTReadExpression>();
        ans = input();
        return ans;
    }else if(currentLexeme.token == Token::LBRACKET){

        eat(Token::LBRACKET,"Expected a LBRACKET");
        auto ans = exprlist();
        eat(Token::RBRACKET,"Expected a RBRACKET");
    }
    else{
        error("Expected a valued token here..");
    }
}

std::shared_ptr<ASTListLiteral> Parser::exprlist() {
    ContextLog clog("exprlist", currentLexeme);
    auto ans = make_shared<ASTListLiteral>();
    //some sort of way to do
    ans->expressions.push_back(expr());
    //exprt();
    return ans;
}
void Parser::exprtail(std::shared_ptr<ASTListLiteral> list) {
    ContextLog clog("exprtail", currentLexeme);
    //TODO
    if(currentLexeme.token == Token::COMMA){
        eat(Token::COMMA,"Expected a comma");
        expr();
        //exprtail();
    }
}

std::shared_ptr<ASTIfStatement> Parser::cond() {
    ContextLog clog("cond", currentLexeme);
    advance();
    auto ans = make_shared<ASTIfStatement>();
    ans->baseIf.expression = bexpr();
    eat(Token::THEN, "Expected THEN");
    //the statements for after the condition are met
    ans->baseIf.statementList = stmts();
    condt(ans);
    eat(Token::END, "Expected END");
    return ans;
}

void Parser::condt(std::shared_ptr<ASTIfStatement> statement) {
    ContextLog clog("condt", currentLexeme);
    if(currentLexeme.token == Token::ELSEIF){
        advance();
        //statement->elseifs.bexpr();
        eat(Token::THEN, "Expected THEN");
        stmts();
        condt(statement);
    }
    else if(currentLexeme.token == Token::ELSE){
        advance();
        stmts();
    }
    else{
        //the empty case, to move on to the next one
    }
}

std::shared_ptr<ASTBoolExpression> Parser::bexpr() {
    ContextLog clog("bexpr", currentLexeme);
    auto boolean_exp = make_shared<ASTComplexBoolExpression>();
    if(currentLexeme.token == Token::NOT){
        boolean_exp->negated = true;
        eat(Token::NOT, "Expected NOT");
        //expr();
        //bexprt();
    }
    else{
        boolean_exp->first = expr();
        if(is_bool_rel()){
            bexprt(boolean_exp);
        }
    }

    return boolean_exp;
}

void Parser::bexprt(std::shared_ptr<ASTComplexBoolExpression> expression) {
    ContextLog clog("bexprt", currentLexeme);
    switch (currentLexeme.token) {
        case Token::EQUAL:
        case Token::LESS_THAN:
        case Token::GREATER_THAN:
        case Token::LESS_THAN_EQUAL:
        case Token::GREATER_THAN_EQUAL:
        case Token::NOT_EQUAL:
            expression->relation = currentLexeme.token;
            break;
        default:
            error("Internal error: expected relational operator");
    }
    advance();
    expression->second = expr();
    if(is_bconnect()){
        bconnect(expression);
    }
}

void Parser::bconnect(std::shared_ptr<ASTComplexBoolExpression> expression) {
    ContextLog clog("bconnect", currentLexeme);
    // TODO
}

std::shared_ptr<ASTWhileStatement> Parser::loop() {
    ContextLog clog("loop", currentLexeme);
    auto ans = make_shared<ASTWhileStatement>();
    // TODO
    return ans;
}



//boolean checks
bool Parser::is_math_rel(){
    if(currentLexeme.token == Token::PLUS){return true;
    }else if(currentLexeme.token == Token::MINUS){return true;
    }else if(currentLexeme.token == Token::DIVIDE){return true;
    }else if(currentLexeme.token == Token::MULTIPLY){return true;
    }else if(currentLexeme.token == Token::MODULUS){return true;
    }else if(currentLexeme.token == Token::EQUAL){return true;
    }
    return false;
}

bool Parser::is_value(){
    if(currentLexeme.token == Token::ID){
        return true;
    }else if(currentLexeme.token == Token::STRING){return true;
    }else if(currentLexeme.token == Token::BOOL){return true;
    }else if(currentLexeme.token == Token::BOOL){return true;
    }else if(currentLexeme.token == Token::LBRACKET){
        // is exper??
    }
}

bool Parser::is_exprt(){
    if(is_math_rel()){
        return true;
    }
    return false;
}

bool Parser::is_stmt(){
    if(currentLexeme.token == Token::PRINTLN || currentLexeme.token == Token::PRINT){
        return true;
    }
    else if(currentLexeme.token == Token::ASSIGN){
        return true;
    }
    else if(currentLexeme.token == Token::IF){
        return true;
    }
    else if(currentLexeme.token == Token::WHILE){
        return true;
    }
    else{
        return false;
    }
}

bool Parser::is_input(){
    if(currentLexeme.token ==Token::READSTR){
        return true;
    }else if(currentLexeme.token == Token::READINT){
        return true;
    }
    return false;
}

bool Parser::is_bool_rel(){
    if(currentLexeme.token == Token::EQUAL){
        return true;
    }else if(currentLexeme.token == Token::LESS_THAN){
        return true;
    }else if(currentLexeme.token == Token::LESS_THAN_EQUAL){
        return true;
    }else if(currentLexeme.token == Token::GREATER_THAN){
        return true;
    }else if(currentLexeme.token == Token::GREATER_THAN_EQUAL){
        return true;
    }else if(currentLexeme.token == Token::NOT_EQUAL){
        return true;
    }
    return false;
}

bool Parser::is_bconnect(){
    if(currentLexeme.token == Token::AND){
        return true;
    }else if(currentLexeme.token == Token::OR){
        return true;
    }
    return false;
};
