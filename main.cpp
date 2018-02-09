#include <iostream>
#include <string>

#include "Token.h"
#include "Lexeme.h"
#include "Lexer.h"
#include "Parser.h"
//#include "P.h"

using namespace std;

int main(int argc, const char * argv[]) {
    // This is a multiline string.  By saying R"code(, we are saying that
    // the string continues until the characters )code" are encountered.
    // You can put your MyPL code between the starting line and the ending
    // line.
    string code = R"code(
x = readint("Enter an int: ");
y = readint("Enter an int: ");
if x > y  AND j == 1 then
    println("The first int was bigger than the second!");
elseif y > x then
    println("The second int was bigger than the first!");
else
    println("You entered the same value twice!");
end
    )code";
    code = R"code(
    if cat == moose then x = 2; max = 2;
end
    )code";
    // Create an istringstream based on the contents of the string "code".
    // We can then use 'ins' just as we would use cin or a file we opened
    // with ifstream.
    istringstream ins(code);

    // Pass the istream to the lexer to have it do its thing.
    Lexer lexer(ins);

    // Create a parser using the lexer
    Parser parser(lexer);

    // Tell the parser to actually do the parsing
    parser.doParse();
    return 0;
}
