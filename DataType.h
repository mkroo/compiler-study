#pragma once
#include <any>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "Node.h"

using std::any;
using std::map;
using std::vector;
using std::string;
using std::ostream;
using std::function;

struct Object {
  bool isMarked = false;
  virtual ~Object() {}
};

struct Array: Object {
  vector<any> values;
};

struct Map: Object {
  map<string, any> values;
};

bool isSize(any value);
size_t toSize(any value);

bool isNull(any value);

bool isTrue(any value);
bool isFalse(any value);
bool isBoolean(any value);
bool toBoolean(any value);

bool isNumber(any value);
double toNumber(any value);

bool isString(any value);
string toString(any value);

bool isArray(any value);
Array* toArray(any value);
any getValueOfArray(any object, any index);
any setValueOfArray(any object, any index, any value);

bool isMap(any value);
Map* toMap(any value);
any getValueOfMap(any object, any key);
any setValueOfMap(any object, any key, any value);

bool isFunction(any);
Function* toFunction(any);

bool isBuiltinFunction(any value);
function<any(vector<any>)> toBuiltinFunction(any value);

ostream& operator<<(ostream& stream, any& value);
