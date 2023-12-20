#include <iostream>
#include <iomanip>
#include "Token.h"

using namespace std;

static string::iterator current;

enum class CharType
{
  Unknown,
  WhiteSpace,
  NumberLiteral,
  StringLiteral,
  IdentifierAndKeyword,
  OperatorAndPunctuator,
};

CharType getCharType(char c) {
  if (' ' == c || '\t' == c || '\n' == c || '\r' == c)
    return CharType::WhiteSpace;
  if ('0' <= c && c <= '9')
    return CharType::NumberLiteral;
  if (c == '\'' )
    return CharType::StringLiteral;
  if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
    return CharType::IdentifierAndKeyword;
  if (33 <= c && c <= 47 && c != '\'' ||
      58 <= c && c <= 64 ||
      91 <= c && c <= 96 ||
      123 <= c && c <= 126)
    return CharType::OperatorAndPunctuator;
  return CharType::Unknown;
}

bool isCharType(char c, CharType charType) {
  switch (charType) {
    case CharType::NumberLiteral:
      return '0' <= c && c <= '9';
    case CharType::StringLiteral:
      return 32 <= c && c <= 126 && c != '\'';
    case CharType::IdentifierAndKeyword:
      return '0' <= c && c <= '9' ||
             'a' <= c && c <= 'z' ||
             'A' <= c && c <= 'Z';
    case CharType::OperatorAndPunctuator:
      return 33 <= c && c <= 47 ||
             58 <= c && c <= 64 ||
             61 <= c && c <= 96 ||
             123 <= c && c <= 126;
    default:
      return false;
  }
}

Token scanNumberLiteral() {
  string string;
  while (isCharType(*current, CharType::NumberLiteral))
    string += *current++;
  if (*current == '.') {
    string += *current++;
    while (isCharType(*current, CharType::NumberLiteral))
      string += *current++;
  }
  return Token{Kind::NumberLiteral, string};
}

Token scanStringLiteral() {
  string string;
  current += 1;
  while (isCharType(*current, CharType::StringLiteral))
    string += *current++;
  
  if (*current != '\'') {
    cout << "문자열의 종료 문자가 없습니다.";
    exit(1);
  }
  current += 1;
  return Token{Kind::StringLiteral, string};
}

Token scanIdentifierAndKeyword() {
  string string;
  while (isCharType(*current, CharType::IdentifierAndKeyword))
    string += *current++;

  Kind kind = toKind(string);
  if (kind == Kind::Unknown)
    kind = Kind::Identifier;
  return Token{Kind::Identifier, string};
}

Token scanOperatorAndPunctuator() {
  string string;
  while (isCharType(*current, CharType::OperatorAndPunctuator))
    string += *current++;

  while (string.empty() == false && toKind(string) == Kind::Unknown) {
    string.pop_back();
    current -= 1;
  }

  if (string.empty())  {
    cout << "사용할 수 없는 문자입니다 .";
    exit(1);
  }
  return Token{toKind(string), string};
}

vector<Token> scan(string sourceCode) {
  vector<Token> result;
  sourceCode += '\0';

  current = sourceCode.begin();
  while (*current != '\0') {
    switch (getCharType(*current)) {
      case CharType::WhiteSpace:
        current += 1;
        break;
      case CharType::NumberLiteral:
        result.push_back(scanNumberLiteral());
        break;
      case CharType::StringLiteral:
        result.push_back(scanStringLiteral());
        break;
      case CharType::IdentifierAndKeyword:
        result.push_back(scanIdentifierAndKeyword());
        break;
      case CharType::OperatorAndPunctuator:
        result.push_back(scanOperatorAndPunctuator());
        break;
      default:
        cout << *current << "사용할 수 없는 문자입니다.";
        exit(1);
      }
  }
  result.push_back({Kind::EndOfToken});
  return result;
}