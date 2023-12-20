#pragma once
#include <string>

using std::string;
using std::ostream;

enum class Kind {
  Unknown, EndOfToken,

  NullLiteral,
  TrueLiteral, FalseLiteral,
  NumberLiteral, StringLiteral,
  Identifier,

  Function, Return,
  Variable,
  For, Break, Continue,
  If, Elif, Else,
  Print, PrintLine,

  LogicalAnd, LogicalOr,
  Assignment,
  Add, Subtract,
  Multiply, Divide, Modulo,
  Equal, NotEqual,
  LessThan, GreaterThan,
  LessOrEqual, GreaterOrEqual,

  Comma, Colon, Semicolon,
  LeftParen, RightParen,
  LeftBrace, RightBrace,
  LeftBraket, RightBraket,
};

struct Token {
  Kind kind = Kind::Unknown;
  string string;
};

Kind toKind(string);
string toString(Kind);

ostream& operator<<(ostream&, Token&);