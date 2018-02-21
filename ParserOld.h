#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Lexer.h"
#include "MyPLException.h"

class ParserException: public MyPLException {
public:
    ParserException(std::string msg=""): MyPLException(msg) {}
};
class Parser {
public:
    // Create parser and initialize with the lexer.  Does not actually
    // perform any parsing
    Parser(Lexer &lex);

    // Run through the parsing process
    void doParse();

    private:
        // Advance to the next lexeme.  Output that lexeme to the screen, and store
        // the lexeme in the member variable "currentLexeme".
        void advance();

        // Utility function I suggest you use.  This function takes in a token
        // that, if the program is correctly formed, will be the next token in the
        // file.  If that token is the next token in the file, this is essentially
        // the same as calling advance once.  If that token ISN'T next in the file,
        // calls error with the given message to throw an exception.
        void eat(Token token, std::string message);

        // Displays an error message and then throws an exception with its string
        // being equal to message plus information on what the current lexeme is.
        //
        // The noreturn attribute tells the compiler that this function will never
        // return normally since it will always throw an exception.  This will help
        // us avoid spurious errors about "control reaching end of non-void
        // function"
        [[noreturn]] void error(std::string message);

        //the organization of this is top-level to the bottom-levelish
        void stmts();
        void stmt();

        //outputting to the user
        void output();

        //assigning a variable some value
        void assign();

        // essentially an if statement
        void cond();

        //the tail of the condition
        void condt();

        // for a while loop
        void loop();

        //represented a generic expression of any sort of thing done to a variable or
        //evaluating the variable
        void expr();

        //the tail of expr, which is above this
        void exprt();

        //checking for a list format
        void exprlist();

        //the tail of the function above, which is exprlist
        void exprtail();

        //boolean expression evaluation
        void bexpr();

        //the tail of the bexpr
        void bexprt();

        //deciphering the contents of a value
        //this can be a multitude of things, as referenced by the language scope
        void value();

        //math operators such as +,-,/,*,%
        void math_rel();

        //connections, which are AND and OR
        void bconnect();

        //evaluating for relational operators, ==, <==, >=, >, <, !=
        void bool_rel();

        //getting input from the user
        //a sublevel of value
        void input();

        //checking for a list format
        //this is used with the assign and the value functions
        void listindex();

        //all of these are checks for either a literal value
        //or for the first lexeme to line up to the other ones.
        bool is_math_rel();
        bool is_bconnect();
        bool is_value();
        bool is_exprt();
        bool is_stmt();
        bool is_input();
        bool is_bool_rel();


        Lexer &lexer;

        Lexeme currentLexeme;
};
#endif /* PARSER_H */
