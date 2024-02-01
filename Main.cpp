#include "Scanner.h"
#include "Parser.h"
#include "Interpreter.h"
#include "Generator.h"

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
  tuple<vector<Code>, map<string, size_t>> objectCode = generate(syntaxTree);
  printObjectCode(objectCode);

  return 0;
}

void printObjectCode(tuple<vector<Code>, map<string, size_t>> objectCode) {
  auto codeList = get<0>(objectCode);
  auto functionTable = get<1>(objectCode);
  cout << setw(11) << left << "FUNCTION" << "ADDRESS" << endl;
  cout << string(18, '-') << endl;
  for (auto& [key, value] : functionTable)
    cout << setw(11) << key << value << endl;
  cout << endl;
  cout << "ADDR" << " " << setw(15) << "INSTRUCTION" << "OPERAND" << endl;
  cout << string(36, '-') << endl;
  for (size_t i = 0; i < codeList.size(); i++)
    cout << setw(4) << right << i << " " << codeList[i] << endl;
}
