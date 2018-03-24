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
    x = [23,3] + [14];
    y[1] = [5];
    z = y + x;
    while 4 != 2 do
        if true then
            j = 4;
        end
    end
    y[0] = [2];

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
