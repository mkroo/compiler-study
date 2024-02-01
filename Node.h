#pragma once
#include <map>
#include <string>
#include <vector>
#include <any>
#include "Token.h"

using std::any;
using std::vector;
using std::map;
using std::string;

struct Program {
  vector<struct Function*> functions;
};

struct Statement {
  virtual void interpret() = 0;
  virtual void generate() = 0;
};

struct Expression {
  virtual any interpret() = 0;
  virtual void generate() = 0;
};

struct Function: Statement {
  string name;
  vector<string> parameters;
  vector<Statement*> block;
  void interpret();
  void generate();
};

struct Variable: Statement {
  string name;
  Expression* expression;
  void interpret();
  void generate();
};

struct Return: Statement {
  Expression* expression;
  void interpret();
  void generate();
};

struct For: Statement {
  Variable* variable;
  Expression* condition;
  Expression* expression;
  vector<Statement*> block;
  void interpret();
  void generate();
};

struct Break: Statement {
  void interpret();
  void generate();
};

struct Continue: Statement {
  void interpret();
  void generate();
};

struct If: Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
  void interpret();
  void generate();
};

struct Print: Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
  void interpret();
  void generate();
};

struct ExpressionStatement: Statement {
  Expression* expression;
  void interpret();
  void generate();
};

struct Or: Expression {
  Expression* lhs;
  Expression* rhs;
  any interpret();
  void generate();
};

struct And: Expression {
  Expression* lhs;
  Expression* rhs;
  any interpret();
  void generate();
};

struct Relational: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  any interpret();
  void generate();
};

struct Arithmetic: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  any interpret();
  void generate();
};

struct Unary: Expression {
  Kind kind;
  Expression* sub;
  any interpret();
  void generate();
};

struct Call: Expression {
  Expression* sub;
  vector<Expression*> arguments;
  any interpret();
  void generate();
};

struct GetElement: Expression {
  Expression* sub;
  Expression* index;
  any interpret();
  void generate();
};

struct SetElement: Expression {
  Expression* sub;
  Expression* index;
  Expression* value;
  any interpret();
  void generate();
};

struct GetVariable: Expression {
  string name;
  any interpret();
  void generate();
};

struct SetVariable: Expression {
  string name;
  Expression* value;
  any interpret();
  void generate();
};

struct NullLiteral: Expression {
  any interpret();
  void generate();
};

struct BooleanLiteral: Expression {
  bool value = false;
  any interpret();
  void generate();
};

struct NumberLiteral: Expression {
  double value = 0.0;
  any interpret();
  void generate();
};

struct StringLiteral: Expression {
  string value;
  any interpret();
  void generate();
};

struct ArrayLiteral: Expression {
  vector<Expression*> values;
  any interpret();
  void generate();
};

struct MapLiteral: Expression {
  map<string, Expression*> values;
  any interpret();
  void generate();
};
