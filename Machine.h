#pragma once
#include <any>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include "DataType.h"
#include "Code.h"

using std::list;
using std::tuple;
using std::vector;
using std::map;
using std::get;
using std::cout;
using std::endl;

struct StackFrame {
  vector<any> variables;
  vector<any> operandStack;
  size_t instructionPointer = 0;
};

static list<Object*> objects;
static map<string, any> global;
static vector<StackFrame> callStack;
extern map<string, function<any(vector<any>)>> builtinFunctionTable;

void execute(tuple<vector<Code>, map<string, size_t>>);