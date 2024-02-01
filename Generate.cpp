
#include <list>
#include "Generator.h"
#include "Token.h"

using std::max;
using std::list;

static size_t getLocal(string);
static void setLocal(string);
static void initBlock();
static void pushBlock();
static void popBlock();
static size_t writeCode(Instruction);
static size_t writeCode(Instruction, any);
static void patchAddress(size_t);
static void patchOperand(size_t, size_t);

static vector<Code> codeList;
static map<string, size_t> functionTable;
static list<map<string, size_t>> symbolStack;
static vector<size_t> offsetStack;
static size_t localSize;
static vector<vector<size_t>> continueStack;
static vector<vector<size_t>> breakStack;

tuple<vector<Code>, map<string, size_t>> generate(Program* program) {
  codeList.clear();
  functionTable.clear();
  writeCode(Instruction::GetGlobal, string("main"));
  writeCode(Instruction::Call, static_cast<size_t>(0));
  writeCode(Instruction::Exit);
  for (auto& node: program->functions)
    node->generate();
  return {codeList, functionTable};
}

void Function::generate() {
  functionTable[name] = codeList.size();
  auto temp = writeCode(Instruction::Alloca);
  initBlock();
  for (auto& name: parameters)
    setLocal(name);
  for (auto& node: block)
    node->generate();
  popBlock();
  patchOperand(temp, localSize);
  writeCode(Instruction::Return);
}

void Variable::generate() {
  setLocal(name);
  expression->generate();
  writeCode(Instruction::SetLocal, getLocal(name));
  writeCode(Instruction::PopOperand);
}

void For::generate() {
  breakStack.emplace_back();
  continueStack.emplace_back();
  pushBlock();
  variable->generate();
  auto jumpAddress = codeList.size();
  condition->generate();
  auto conditionJump = writeCode(Instruction::ConditionJump);
  for (auto& node: block)
    node->generate();
  auto continueAddress = codeList.size();
  expression->generate();
  writeCode(Instruction::PopOperand);
  writeCode(Instruction::Jump, jumpAddress);
  patchAddress(conditionJump);
  popBlock();
  for (auto& jump: continueStack.back())
    patchOperand(jump, continueAddress);
  continueStack.pop_back();
  for (auto& jump: breakStack.back())
    patchAddress(jump);
  breakStack.pop_back();
}

void Continue::generate() {
  if (continueStack.empty()) return;
  auto jumpCode = writeCode(Instruction::Jump);
  continueStack.back().push_back(jumpCode);
}

void Break::generate() {
  if (breakStack.empty()) return;
  auto jumpCode = writeCode(Instruction::Jump);
  breakStack.back().push_back(jumpCode);
}

void If::generate() {
  vector<size_t> jumpList;
  for (size_t i = 0; i < conditions.size(); i++) {
    conditions[i]->generate();
    auto conditionJump = writeCode(Instruction::ConditionJump);
    pushBlock();
    for (auto& node: blocks[i])
      node->generate();
    popBlock();
    jumpList.push_back(writeCode(Instruction::Jump));
    patchAddress(conditionJump);
  }
  if (elseBlock.empty() == false) {
    pushBlock();
    for (auto& node: elseBlock)
      node->generate();
    popBlock();
  }
  for (auto& jump: jumpList)
    patchAddress(jump);
}

void Print::generate() {
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1]->generate();
  writeCode(Instruction::Print, arguments.size());
  if (lineFeed)
    writeCode(Instruction::PrintLine);
}

void Return::generate() {
  expression->generate();
  writeCode(Instruction::Return);
}

void ExpressionStatement::generate() {
  expression->generate();
  writeCode(Instruction::PopOperand);
}

void Or::generate() {
  lhs->generate();
  auto logicalOr = writeCode(Instruction::LogicalOr);
  rhs->generate();
  patchAddress(logicalOr);
}

void And::generate() {
  lhs->generate();
  auto logicalAnd = writeCode(Instruction::LogicalAnd);
  rhs->generate();
  patchAddress(logicalAnd);
}

void Relational::generate() {
  map<Kind, Instruction> instructions = {
    {Kind::Equal,          Instruction::Equal},
    {Kind::NotEqual,       Instruction::NotEqual},
    {Kind::LessThan,       Instruction::LessThan},
    {Kind::GreaterThan,    Instruction::GreaterThan},
    {Kind::LessOrEqual,    Instruction::LessOrEqual},
    {Kind::GreaterOrEqual, Instruction::GreaterOrEqual}
  };
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}

void Arithmetic::generate() {
  map<Kind, Instruction> instructions = {
    {Kind::Add,      Instruction::Add},
    {Kind::Subtract, Instruction::Subtract},
    {Kind::Multiply, Instruction::Multiply},
    {Kind::Divide,   Instruction::Divide},
    {Kind::Modulo,   Instruction::Modulo},
  };
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}

void Unary::generate() {
  map<Kind, Instruction> instructions = {
    {Kind::Add,      Instruction::Absolute},
    {Kind::Subtract, Instruction::ReverseSign}
  };
  sub->generate();
  writeCode(instructions[kind]);
}

void Call::generate() {
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1]->generate();
  sub->generate();
  writeCode(Instruction::Call, arguments.size());
}

void GetElement::generate() {
  sub->generate();
  index->generate();
  writeCode(Instruction::GetElement);
}

void SetElement::generate() {
  value->generate();
  sub->generate();
  index->generate();
  writeCode(Instruction::SetElement);
}

void NullLiteral::generate() {
  writeCode(Instruction::PushNull);
}

void BooleanLiteral::generate() {
  writeCode(Instruction::PushBoolean, value);
}

void NumberLiteral::generate() {
  writeCode(Instruction::PushNumber, value);
}

void StringLiteral::generate() {
  writeCode(Instruction::PushString, value);
}

void ArrayLiteral::generate() {
  for (auto i = values.size(); i > 0; i--)
    values[i - 1]->generate();
  writeCode(Instruction::PushArray, values.size());
}

void MapLiteral::generate() {
  for (auto& [key, value]: values) {
    writeCode(Instruction::PushString, key);
    value->generate();
  }
  writeCode(Instruction::PushMap, values.size());
}

void GetVariable::generate() {
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::GetGlobal, name);
  else
    writeCode(Instruction::GetLocal, getLocal(name));
}

void SetVariable::generate() {
  value->generate();
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::SetGlobal, name);
  else
    writeCode(Instruction::SetLocal, getLocal(name));
}

size_t getLocal(string name) {
  for (auto& symbolTable: symbolStack) {
    if (symbolTable.count(name))
      return symbolTable[name];
  }
  return SIZE_MAX;
}

void setLocal(string name) {
  symbolStack.front()[name] = offsetStack.back();
  offsetStack.back() += 1;
  localSize = max(localSize, offsetStack.back());
}

void initBlock() {
  localSize = 0;
  offsetStack.push_back(0);
  symbolStack.emplace_front();
}

void pushBlock() {
  symbolStack.emplace_front();
  offsetStack.push_back(offsetStack.back());
}

void popBlock() {
  offsetStack.pop_back();
  symbolStack.pop_front();
}

size_t writeCode(Instruction instruction) {
  codeList.push_back({instruction});
  return codeList.size() - 1;
}

size_t writeCode(Instruction instruction, any operand) {
  codeList.push_back({instruction, operand});
  return codeList.size() - 1;
}

void patchAddress(size_t codeIndex) {
  codeList[codeIndex].operand = codeList.size();
}

void patchOperand(size_t codeIndex, size_t operand) {
  codeList[codeIndex].operand = operand;
}
