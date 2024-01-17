#include "Scanner.h"
#include "Parser.h"
#include "Interpreter.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

int main() {
  string sourceCode = R""""(
    function main() {
      for i = 0, i < 10, i = i + 1 {
        printLine i;
      }
      print 'Hello, World!';
    }
  )"""";

  vector<Token> tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  return 0;
}
