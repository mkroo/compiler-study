#include "Scanner.h"
#include "Parser.h"
#include "Printer.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

int main() {
  string sourceCode = R""""(
    function main() {
      print 'Hello, World!';
    }
  )"""";

  vector<Token> tokenList = scan(sourceCode);
  printTokenList(tokenList);

  cout << "--------------" << endl;
  
  auto syntaxTree = parse(tokenList);
  printSyntaxTree(syntaxTree);

  return 0;
}
