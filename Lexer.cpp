#include <sstream>
#include <iostream>
#include "Lexer.h"
#include <stdexcept>

/*
Maxwell Dulin
Lexer Fun!
*/
using namespace std;

Lexer::Lexer(std::istream &initIn): in(initIn) {
    // You shouldn't need to modify this function

    // If we are given an invalid input stream, throw an exception
    if (!in) {
        throw LexerException("Invalid input stream passed to Lexer");
    }
}

Lexeme Lexer::next() {

    eatWhitespace();
    char tmp = peek();
    int startLine = lineNum;
    int startCol = colNum;
    if (tmp == eof()) {
        return Lexeme(Token::EOS, "", startLine, startCol);
    } else if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')) {
        return runWord(startLine,startCol);
    } else if (tmp == '+') {
        //should I be peeking ahead to see if anything else is going on here?
        //like an incorrect character?
        return makeSingleCharLexeme(Token::PLUS,startLine, startCol);
    } else if(tmp == '-') {
        return makeSingleCharLexeme(Token::MINUS,startLine, startCol);
    } else if(tmp == '*') {
        return makeSingleCharLexeme(Token::MULTIPLY,startLine, startCol);
    } else if(tmp == '/') {
        return makeSingleCharLexeme(Token::DIVIDE,startLine, startCol);
    } else if(tmp == '%') {
        return makeSingleCharLexeme(Token::MODULUS,startLine, startCol);
    } else if (tmp == '>') {
        read();
        if (peek() == '=') {
            read();
            return Lexeme(Token::GREATER_THAN_EQUAL, ">=", startLine, startCol);
        } else {
            return Lexeme(Token::GREATER_THAN, ">", startLine, startCol);
        }
    }else if(isLiteral(tmp)){
        return literally(startLine, startCol);
    } else if (tmp == '<') {
        read();
        if (peek() == '=') {
            read();
            return Lexeme(Token::LESS_THAN_EQUAL, "<=", startLine, startCol);
        } else {
            return Lexeme(Token::LESS_THAN, "<", startLine, startCol);
        }
    } else if(tmp == '='){
        read();
        if(peek() == '='){
            read();
            return Lexeme(Token::EQUAL, "==", startLine, startCol);
        }else{
            return Lexeme(Token::ASSIGN, "=", startLine, startCol);
        }
    }else if(tmp == '!'){
        read();
        if(peek() == '='){
            read();
            return Lexeme(Token::NOT_EQUAL, "!=", startLine, startCol);
        }else{
            ostringstream oss;
            oss << "Expecting = got '" << peek() << "'";
            throw LexerException(debugMessage(oss.str()));
        }
    }else if (tmp == '(' || tmp == ')' || tmp == '[' or tmp == ']'){

        if(tmp == '('){
            return makeSingleCharLexeme(Token::LPAREN,startLine, startCol);
        }else if(tmp == ')'){
            return makeSingleCharLexeme(Token::RPAREN,startLine, startCol);
        }else if(tmp == '['){
            return makeSingleCharLexeme(Token::LBRACKET,startLine, startCol);
        }else if(tmp == ']'){
            return makeSingleCharLexeme(Token::RBRACKET,startLine, startCol);
        }
    }
    else if(tmp == ';'){
        return makeSingleCharLexeme(Token::SEMICOLON,startLine, startCol);
    }
    else if(tmp == ','){
        return makeSingleCharLexeme(Token::COMMA,startLine, startCol);
    }
    //need to check for brackets and braces now!
    else {
        // You will not need to change the contents of this "else"
        ostringstream oss;
        oss << "Unexpected symbol: '" << peek() << "'";
        throw LexerException(debugMessage(oss.str()));
    }
}

bool Lexer::atEOF() {
    // You shouldn't need to modify this function
    return peek() == eof();
}

Lexeme Lexer::literally(int lineVal, int colVal){
    char tmp = peek();
    bool go = false;
    string str = "";
    str += in.peek();
    if(isDigit(tmp)){
        read();

        while(go == false){
            if(isDigit(peek())){
                str +=in.peek();
            }
            //what to do with an error here?
            else if(peek() == ',' || peek() == ' ' || peek() == ';' || peek() == ']' || peek() == ')'){
                return Lexeme(Token::INT, str, lineVal, colVal);
            }else{
                ostringstream oss;
                oss << "Unexpected symbol for integer literal: '" << peek() << "'";
                throw LexerException(debugMessage(oss.str()));
            }
            read();
        }
    }else if(tmp == '"'){
        read();
        while(go == false){
            if(peek() != '"' && peek() != '\n'){
                str +=in.peek();
            }
            else if(peek() == '\n'){
                ostringstream oss;
                oss << "Non terminated string:" << "'";
                throw LexerException(debugMessage(oss.str()));
            }
            else{
                str +=in.peek();
                read();
                return Lexeme(Token::STRING, str, lineVal, colVal);
            }
            read();
        }
    }
}

char Lexer::peek() {
    // You shouldn't need to modify this function
    return in.peek();
}

bool Lexer::isChar(){
    //Checks to see if the character is legal for a variable.
    char tmp = in.peek();
    if((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')|| (tmp >= '0' && tmp <= '9')|| tmp =='_'){
        return true;
    }else{
        return false;
    }
}

bool Lexer::isKeyword(string word){
    if(word == "while" || word == "do" || word == "end"){
        return true;
    }else if(word == "print" || word == "println" || word == "semicolon"){
        return true;
    }else if(word == "readint" || word == "readstr"){
        return true;
    }else if(word == "or" || word == "not" || word == "and"){
        return true;
    }else if(word == "true" || word == "false"){
        return true;
    }else if(word == "if" || word =="then" || word == "elseif" || word == "else"){
        return true;
    }else{
        return false;
    }
}

Lexeme Lexer::runWord(int startLine, int startCol){
    int lineBegin = startLine;
    int colBegin = startCol;
    char tmp;
    string var = "";
    var += in.peek();
    read();
    bool go = false;
    while(go == false){
        tmp = peek();
        if(tmp == '\n' || tmp == ' ' || tmp == '\t' || tmp == ';' || tmp == '(' || tmp == '[' || tmp == ']' || tmp == ')' || tmp == ','){
            //checks for keywords
            bool word = isKeyword(var);
            if(word == true && tmp != '(' && tmp != ')' && tmp != ']' && tmp != ';' && tmp != ','){
                read();
                return getKeyword(var,lineBegin,colBegin);
            }else if(word == true){
                return getKeyword(var,lineBegin, colBegin);
            }
            return Lexeme(Token::ID, var, lineBegin, colBegin);
        }
        var += tmp;
        if(isChar()==false){
            ostringstream oss;
            oss << "Unexpected symbol: '" << peek() << "'";
            throw LexerException(debugMessage(oss.str()));
        }
        read();
    }
}

Lexeme Lexer::getKeyword(string word, int lineNum, int colNum){
    if(word == "while"){
        return Lexeme(Token::WHILE, word, lineNum, colNum);
    }else if(word == "do"){
        return Lexeme(Token::DO, word, lineNum, colNum);
    }else if(word == "end"){
        return Lexeme(Token::END, word, lineNum, colNum);
    }else if(word == "print"){
        return Lexeme(Token::PRINT, word, lineNum, colNum);
    }else if(word == "println"){
        return Lexeme(Token::PRINTLN, word, lineNum, colNum);
    }else if(word == "readint"){
        return Lexeme(Token::READINT, word, lineNum, colNum);
    }else if(word == "readstr"){
        return Lexeme(Token::READSTR, word, lineNum, colNum);
    }else if(word == "and"){
        return Lexeme(Token::AND, word, lineNum, colNum);
    }else if(word == "not"){
        return Lexeme(Token::NOT, word, lineNum, colNum);
    }else if(word == "or"){
        return Lexeme(Token::OR, word, lineNum, colNum);
    }else if(word == "true"){
        return Lexeme(Token::BOOL, word, lineNum, colNum);
    }else if(word == "false"){
        return Lexeme(Token::BOOL, word, lineNum, colNum);
    }else if(word == "if"){
        return Lexeme(Token::IF, word, lineNum, colNum);
    }else if(word == "then"){
        return Lexeme(Token::THEN, word, lineNum, colNum);
    }else if(word == "elseif"){
        return Lexeme(Token::ELSEIF, word, lineNum, colNum);
    }else if(word == "else"){
        return Lexeme(Token::ELSE, word, lineNum, colNum);
    }

}

char Lexer::read() {

    if (in.peek() == '\n') {
        colNum = 1;
        lineNum += 1;
    } else {
        colNum += 1;
    }
    return in.get();
}
void Lexer::eatWhitespace() {
    bool go = false;
    while(go == false){
        char tmp = peek();
        //for all whitespace
        if(tmp == '\n' || tmp == ' ' || tmp == '\t' || tmp == '\r'){
            read();
            char tmp2 = peek();
        //for comments
        }else if(tmp == '#'){
            //eat up everything in the line
            read();
            int catchLine = lineNum;
            while(lineNum == catchLine){
                read();
            }
        }else{
            go = true;
        }
    }
}

bool Lexer::isDigit(char chr){
    //checks to see if a number is a digit or not
    if(chr >= '0' && chr <= '9'){
        return true;
    }
    return false;
}

bool Lexer::isLiteral(char chr){
    //check for strings, int's, bool,
    if(chr == '"' || isDigit(chr)){
        return true;
    }
    return false;
}

char Lexer::eof() {
    // You shouldn't need to modify this function
    return std::istream::traits_type::eof();
}

Lexeme Lexer::makeSingleCharLexeme(Token token, int startLine, int startCol) {
    // You shouldn't need to modify this function
    string str(1, read());
    return Lexeme(token, str, startLine, startCol);
}

std::string Lexer::debugMessage(std::string msg) {
    // You shouldn't need to modify this function
    ostringstream oss;
    oss << msg << " " << lineNum << ":" << colNum;
    return oss.str();
}
