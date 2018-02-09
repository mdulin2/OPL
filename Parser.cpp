#include "Parser.h"

#include <iostream>
#include <sstream>

using namespace std;

// Utility class to make it easier to display messages when entering or
// leaving a non-terminal function.  Create an object of type ContextLog at
// the beginning of your function, passing a string with the current
// nonterminal name and the current lexeme into the constructor.  This class
// will then output an "Entering" message immediately, and an "Exiting" message
// when the function is exited.
class ContextLog {
public:
    ContextLog(string p, const Lexeme &lex):
    part(p),
    l(lex.line),
    c(lex.col) {
        cout << "Entering " << part << "(" << l << ":" << c << ")" << endl;
    }
    ~ContextLog() {
        cout << "Exiting " << part << "(" << l << ":" << c << ")" << endl;
    }
private:
    string part;
    int l;
    int c;
};

Parser::Parser(Lexer &lex): lexer(lex) {
    // We need to call "advance" once at the beginning to get the first lexeme
    // from the lexer
    advance();
}

void Parser::doParse() {
    // The top-level nonterminal of our language is "stmts" so we just need
    // to call this once to parse the entire program.
    stmts();
}

void Parser::advance() {
    // You will not need to change this function
    currentLexeme = lexer.next();
    cout << currentLexeme << endl;
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

void Parser::stmts() {
    ContextLog clog("stmts", currentLexeme);
    switch (currentLexeme.token) {
        case Token::PRINT:
        case Token::PRINTLN:
        case Token::ID:
        case Token::IF:
        case Token::WHILE:
            stmt();
            stmts();
            break;
        default:
            // May be empty
            break;
    }
}

// From here on, you will want to modify these functions
void Parser::stmt() {
    ContextLog clog("stmt", currentLexeme);
    //expr();
    switch (currentLexeme.token) {
        case Token::PRINT:
        case Token::PRINTLN:
            output();
            break;
        case Token::ID:
            assign();
            break;
        case Token::IF:
            cond();
        case Token::WHILE:
            //loop
        default:
            break;
        }
}

void Parser::bexpr(){
    ContextLog clog("bexpr", currentLexeme);
    cout << "fine and dandy!" << endl;
    if(currentLexeme.token != Token::NOT){
        expr();
        bexprt();
    }
    else{
        eat(Token::NOT, "Expected NOT");
        expr();
        bexprt();
    }
}

void Parser::bexprt(){
    ContextLog clog("bexprt", currentLexeme);
    if(is_bool_rel()){
        //and's and or's will break this badly...
        bool_rel();
        expr();
        bconnect();
    }
    else{
        //empty case
    }
}

//condition______________
void Parser::cond(){
    ContextLog clog("cond", currentLexeme);
    advance();
    bexpr();
    eat(Token::THEN, "Expected THEN");
    stmts();
    condt();
    eat(Token::END, "Expected END");
}

//condition tail******
void Parser::condt(){
    ContextLog clog("condt", currentLexeme);
    if(currentLexeme.token == Token::ELSEIF){
        advance();
        bexpr();
        eat(Token::THEN, "Expected THEN");
        stmts();
        condt();
    }
    else if(currentLexeme.token == Token::ELSE){
        advance();
        stmts();
    }
    else{
        //the empty case, to move on to the next one
    }
}

void Parser::assign(){
    ContextLog clog("ASSIGN", currentLexeme);
    advance();
    listindex();
    eat(Token::ASSIGN,"expected assignment");
    expr();
    eat(Token::SEMICOLON,"expected semicolon");

}

void Parser::output() {
    ContextLog clog("output", currentLexeme);
    if (currentLexeme.token == Token::PRINT) {
        eat(Token::PRINT, "Expected print");
    } else {
        eat(Token::PRINTLN, "Expected print or println");
    }
    eat(Token::LPAREN, "Expected '('");
    expr();
    eat(Token::RPAREN, "Expected ')'");
    eat(Token::SEMICOLON, "Expected ';'");
}

void Parser::listindex() {
    ContextLog clog("listindex", currentLexeme);
    if (currentLexeme.token == Token::LBRACKET) {
        advance();
        expr();
        eat(Token::RBRACKET, "Expected ']'");
    }
    else{
        //this is the case of the empty listindex
    }
}

void Parser::input() {
    ContextLog clog("input", currentLexeme);
    if(is_input()){
        advance();
    }
    else{
        error("Expected an input value");
    }
    eat(Token::LPAREN,"Expected a LPAREN");
    eat(Token::STRING,"Expected a STRING");
    eat(Token::RPAREN,"Expected a RPAREN");
}

void Parser::expr() {
    ContextLog clog("expr", currentLexeme);

    //why is this order not followed in the example?
    //But, assigning stil goes into listindex?
    value();
    exprt();
}

void Parser::math_rel(){
    ContextLog clog("exprt", currentLexeme);
    if(currentLexeme.token == Token::PLUS){
    }else if(currentLexeme.token == Token::MINUS){
    }else if(currentLexeme.token == Token::DIVIDE){
    }else if(currentLexeme.token == Token::MULTIPLY){
    }else if(currentLexeme.token == Token::MODULUS){
    }else{
        abort();
    }
    advance();
    return;
}

void Parser::exprt(){
    ContextLog clog("exprt", currentLexeme);

    // +,-...
    if(is_math_rel()){
        math_rel();
        expr();
        return;
    }
    else{
        //no more to read in the expression
        return;
    }

}

void Parser::value(){
    ContextLog clog("value", currentLexeme);
    //ID
    if(currentLexeme.token == Token::ID){
        eat(Token::ID,"Expected an ID");
        return;
    //string
    }else if(currentLexeme.token == Token::STRING){
        eat(Token::STRING,"Expected a string");
        return;
    //boolean
    }else if(currentLexeme.token == Token::BOOL){
        eat(Token::BOOL,"Expected a boolean");
        return;
    //integer
    }else if(currentLexeme.token == Token::INT){
        eat(Token::INT,"Expected an INT");
        return;
    //input
    }else if(is_input()){
        input();
    //listindex
    }else if(currentLexeme.token == Token::LBRACKET){
        eat(Token::LBRACKET,"Expected a LBRACKET");
        exprlist();
        eat(Token::RBRACKET,"Expected a RBRACKET");
    }
    else{
        eat(Token::ID,"Expected an ID or literal");
        abort();
    }
}

//this is a list!
void Parser::exprlist(){
    ContextLog clog("exprlist", currentLexeme);
    expr();
    exprtail();

}

void Parser::exprtail(){
    ContextLog clog("exprtail", currentLexeme);
    if(currentLexeme.token == Token::COMMA){
        eat(Token::COMMA,"Expected a comma");
        expr();
        exprtail();
    }

}

void Parser::bconnect(){
    ContextLog clog("bconnect", currentLexeme);
    if(currentLexeme.token == Token::AND){
        advance();
        bexpr();
        return;
    }else if(currentLexeme.token == Token::OR){
        advance();
        bexpr();
        return;
    }else {
        return;
        //empty
    }
}

void Parser::bool_rel(){
    ContextLog clog("bool_rel", currentLexeme);
    if(currentLexeme.token == Token::EQUAL){
        advance();
        return;
    }else if(currentLexeme.token == Token::LESS_THAN){
        advance();
        return;
    }else if(currentLexeme.token == Token::LESS_THAN_EQUAL){
        advance();
        return;
    }else if(currentLexeme.token == Token::GREATER_THAN){
        advance();
        return;
    }else if(currentLexeme.token == Token::GREATER_THAN_EQUAL){
        advance();
        return;
    }else if(currentLexeme.token == Token::NOT_EQUAL){
        advance();
        return;
    }
}


//boolean checks
bool Parser::is_math_rel(){
    if(currentLexeme.token == Token::PLUS){return true;
    }else if(currentLexeme.token == Token::MINUS){return true;
    }else if(currentLexeme.token == Token::DIVIDE){return true;
    }else if(currentLexeme.token == Token::MULTIPLY){return true;
    }else if(currentLexeme.token == Token::MODULUS){return true;}
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
}

bool Parser::is_bconnect(){
    if(currentLexeme.token == Token::AND){
        return true;
    }else if(currentLexeme.token == Token::OR){
        return true;
    }
    return false;
};
