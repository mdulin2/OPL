#include <iostream>
#include <string>

#include "Token.h"
#include "Lexeme.h"
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Typechecker.h"

using namespace std;

int main(int argc, const char * argv[]) {
    string code = R"code(
    max = 5;
    t = true;
    st = "string";
    if 5 == max then
        t = false;
    elseif t then
        x = 5;
        x[0] = max;
    else
        max = 8;
      end

     g = 5 + 11 * 5 / 54 % 12;
     g = "t" - "hh";
    )code";

    string x = "x = 5 + 111; ";
    try {
        istringstream ins(code);
        Lexer lexer(ins);
        Parser parser(lexer);
        ASTPrinter printer;
        parser.getAST()->accept(printer);
        Typechecker checker;
        parser.getAST()->accept(checker);
    } catch (MyPLException &e) {
        cout << "Error encountered: " << e.what() << endl;
    }
    return 0;
}
