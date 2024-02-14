#include "Machine.h"
#include "GarbageCollecter.h"

static void pushOperand(any value);
static any peekOperand();
static any popOperand();

void execute(tuple<vector<Code>, map<string, size_t>> objectCode) {
  global.clear();
  objects.clear();
  callStack.emplace_back();

  vector<Code> codeList = get<0>(objectCode);
  map<string, size_t> functionTable = get<1>(objectCode);

  while (true) {
    Code code = codeList[callStack.back().instructionPointer];

    switch (code.instruction) {
    case Instruction::Exit: {
      callStack.pop_back();
      return;
    }
    case Instruction::Call: {
      auto operand = popOperand();
      if (isSize(operand)) {
        StackFrame stackFrame;
        stackFrame.instructionPointer = toSize(operand);
        for (size_t i = 0; i < toSize(code.operand); i++) {
          stackFrame.variables.push_back(callStack.back().operandStack.back());
          callStack.back().operandStack.pop_back();
        }
        callStack.push_back(stackFrame);
        continue;
      }
      if (isBuiltinFunction(operand)) {
        vector<any> arguments;
        for (size_t i = 0; i < toSize(code.operand); i++)
          arguments.push_back(popOperand());
        pushOperand(toBuiltinFunction(operand)(arguments));
        break;
      }
      pushOperand(nullptr);
      break;
    }
    case Instruction::Alloca: {
      auto extraSize = toSize(code.operand);
      auto currentSize = callStack.back().variables.size();
      callStack.back().variables.resize(currentSize + extraSize);
      break;
    }
    case Instruction::Return: {
      any result = nullptr;
      if (callStack.back().operandStack.empty() == false)
        result = callStack.back().operandStack.back();
      callStack.pop_back();
      callStack.back().operandStack.push_back(result);
      collectGarbage();
      break;
    }
    case Instruction::Jump: {
      callStack.back().instructionPointer = toSize(code.operand);
      continue;
    }
    case Instruction::ConditionJump: {
      auto condition = popOperand();
      if (isTrue(condition))
        break;
      callStack.back().instructionPointer = toSize(code.operand);
      continue;
    }
    case Instruction::Print: {
      for (size_t i = 0; i < toSize(code.operand); i++) {
        auto value = popOperand();
        cout << value;
      }
      break;
    }
    case Instruction::PrintLine: {
      cout << endl;
      break;
    }
    case Instruction::LogicalOr: {
      auto value = popOperand();
      if (isTrue(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::LogicalAnd: {
      auto value = popOperand();
      if (isFalse(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::Equal: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNull(lValue) && isNull(rValue))
        pushOperand(true);
      else if (isBoolean(lValue) && isBoolean(rValue))
        pushOperand(toBoolean(lValue) == toBoolean(rValue));
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) == toNumber(rValue));
      else if (isString(lValue) && isString(rValue))
        pushOperand(toString(lValue) == toString(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::NotEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNull(lValue) && isNull(rValue))
        pushOperand(false);
      else if (isNull(lValue) || isNull(rValue))
        pushOperand(true);
      if (isBoolean(lValue) && isBoolean(rValue))
        pushOperand(toBoolean(lValue) != toBoolean(rValue));
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) != toNumber(rValue));
      else if (isString(lValue) && isString(rValue))
        pushOperand(toString(lValue) != toString(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::LessThan: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) < toNumber(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::GreaterThan: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) > toNumber(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::LessOrEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) <= toNumber(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::GreaterOrEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) >= toNumber(rValue));
      else
        pushOperand(false);
      break;
    }
    case Instruction::Add: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) + toNumber(rValue));
      else if (isString(lValue) && isString(rValue))
        pushOperand(toString(lValue) + toString(rValue));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::Subtract: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) - toNumber(rValue));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::Multiply: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) * toNumber(rValue));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::Divide: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue) && toNumber(rValue) == 0)
        pushOperand(0.0);
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) / toNumber(rValue));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::Modulo: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue) && toNumber(rValue) == 0)
        pushOperand(0.0);
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(fmod(toNumber(lValue), toNumber(rValue)));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::Absolute: {
      auto value = popOperand();
      if (isNumber(value))
        pushOperand(abs(toNumber(value)));
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::ReverseSign: {
      auto value = popOperand();
      if (isNumber(value))
        pushOperand(toNumber(value) * -1);
      else
        pushOperand(0.0);
      break;
    }
    case Instruction::GetElement: {
      auto index = popOperand();
      auto sub = popOperand();
      if (isArray(sub) && isNumber(index))
        pushOperand(getValueOfArray(sub, index));
      else if (isMap(sub) && isString(index))
        pushOperand(getValueOfMap(sub, index));
      else
        pushOperand(nullptr);
      break;
    }
    case Instruction::SetElement: {
      auto index = popOperand();
      auto sub = popOperand();
      if (isArray(sub) && isNumber(index))
        setValueOfArray(sub, index, peekOperand());
      else if (isMap(sub) && isString(index))
        setValueOfMap(sub, index, peekOperand());
      break;
    }
    case Instruction::GetGlobal: {
      auto name = toString(code.operand);
      if (functionTable.count(name))
        pushOperand(functionTable[name]);
      else if (builtinFunctionTable.count(name))
        pushOperand(builtinFunctionTable[name]);
      else if (global.count(name))
        pushOperand(global[name]);
      else
        pushOperand(nullptr);
      break;
    }
    case Instruction::SetGlobal: {
      auto name = toString(code.operand);
      global[name] = peekOperand();
      break;
    }
    case Instruction::GetLocal: {
      auto index = toSize(code.operand);
      pushOperand(callStack.back().variables[index]);
      break;
    }
    case Instruction::SetLocal: {
      auto index = toSize(code.operand);
      callStack.back().variables[index] = peekOperand();
      break;
    }
    case Instruction::PushNull: {
      pushOperand(nullptr);
      break;
    }
    case Instruction::PushBoolean: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushNumber: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushString: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushArray: {
      auto result = new Array();
      auto size = toSize(code.operand);
      for (auto i = size; i > 0; i--)
        result->values.push_back(popOperand());
      pushOperand(result);
      objects.push_back(result);
      break;
    }
    case Instruction::PushMap: {
      auto result = new Map();
      for (size_t i = 0; i < toSize(code.operand); i++) {
        auto value = popOperand();
        auto key = toString(popOperand());
        result->values[key] = value;
      }
      pushOperand(result);
      objects.push_back(result);
      break;
    }
    case Instruction::PopOperand: {
      popOperand();
      break;
    }
    }
    callStack.back().instructionPointer += 1;
  }
}

any popOperand() {
  auto value = callStack.back().operandStack.back();
  callStack.back().operandStack.pop_back();
  return value;
}

void pushOperand(any value) {
  callStack.back().operandStack.push_back(value); // TODO: fix
}

any peekOperand() {
  return callStack.back().operandStack.back();
}