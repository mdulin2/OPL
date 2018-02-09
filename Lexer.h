#ifndef LEXER_H
#define LEXER_H

#include <iosfwd>
#include <string>
#include <exception>

#include "Token.h"
#include "Lexeme.h"
#include "MyPLException.h"

// Lexer exception class, to be thrown when an error is encountered in lexing
class LexerException: public MyPLException {
public:
    LexerException(std::string msg=""): MyPLException(msg) {}
};

// Class responsible for lexing input.
class Lexer {
public:
    // Create a Lexer given the specified input stream to read characters from
    Lexer(std::istream& initIn);

    // Return the next Lexeme.  Returns Lexeme::EOS if at the end of the input
    Lexeme next();

    // Returns true if at the end of the input, false otherwise.
    bool atEOF();
private:
    // Returns the next character in the input.  If the end of the input has
    // been reached, returns a value equal to the result of calling the eof()
    // function
    char peek();

    // Reads the next character from the input, updating lineNum and colNum
    // as appropriate
    char read();

    // Suggested: Reads in until a non-whitespace character is reached
    void eatWhitespace();

    // Returns the value that peek() and read() will return when at the end of
    // the input
    char eof();

    // Many Tokens are always represented by a single character.  This function
    // can be useful to make those.
    Lexeme makeSingleCharLexeme(Token token, int startLine, int startCol);

    // Given an error message, appends the line and column number so that the
    // result can then be passed to a LexerException to be thrown.
    std::string debugMessage(std::string msg);

    // Returns true if the character is legal for a variable name.
    // False otherwise.
    bool isChar();

    //Returns true if the word is a keyword.
    //False otherwise
    bool isKeyword(std::string word);

    //Returns true if the characer is the start of a literal
    //False otherwise
    bool isLiteral(char chr);

    //Returns true if the character is a digit, false otherwise
    bool isDigit(char chr);

    //gets the Lexeme associated with the keyword
    Lexeme getKeyword(std::string word, int lineBegin, int colBegin);

    //gets the output for the word section.
    Lexeme runWord(int startLine, int startCol);

    //gets the output for the literal section.
    Lexeme literally(int startLine, int startCol);
    // Stream to read characters from.  You should not need to call any
    // functions on this variable, instead using peek() and read() to get
    // the data you need.
    std::istream &in;

    // What line contains the character that "peek" will return
    int lineNum = 1;

    // What column contains the character that "peek" will return
    int colNum = 1;
};

#endif /* LEXER_H */
