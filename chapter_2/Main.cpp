#include <iostream>
#include <iomanip>
#include "Token.h"

using namespace std;

vector<Token> scan(string);

void printTokenList(vector<Token> tokenList) {
  for (Token token : tokenList) {
    cout << setw(20) << left << toString(token.kind) << token.string << endl;
  }
}

int main() {
  string sourceCode = R""""(
    function main() {
      printLine 'Hello, World!';
      printLine 1 + 2 * 3;
    }
  )"""";
  vector<Token> tokenList = scan(sourceCode);
  printTokenList(tokenList);

  return 0;
}
