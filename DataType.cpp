#include <iomanip>
#include <iostream>
#include "DataType.h"

using std::ostream;
using std::any_cast;
using std::boolalpha;

bool isSize(any value) {
  return value.type() == typeid(size_t);
}

size_t toSize(any value) {
  return any_cast<size_t>(value);
}

bool isNull(any value) {
  return value.type() == typeid(nullptr_t);
}

bool isTrue(any value) {
  return isBoolean(value) && toBoolean(value);
}

bool isFalse(any value) {
  return isBoolean(value) && toBoolean(value) == false;
}

bool isBoolean(any value) {
  return value.type() == typeid(bool);
}

bool toBoolean(any value) {
  return any_cast<bool>(value);
}

bool isNumber(any value) {
  return value.type() == typeid(double);
}

double toNumber(any value) {
  return any_cast<double>(value);
}

bool isString(any value) {
  return value.type() == typeid(string);
}

string toString(any value) {
  return any_cast<string>(value);
}

bool isArray(any value) {
  return value.type() == typeid(Array*);
}

Array* toArray(any value) {
  return any_cast<Array*>(value);
}

any getValueOfArray(any object, any index) {
  int i = toNumber(index);
  if (i >= 0 && i < toArray(object)->values.size())
    return toArray(object)->values[i];
  return nullptr;
}

any setValueOfArray(any object, any index, any value) {
  int i = toNumber(index);
  if (i >= 0 && i < toArray(object)->values.size())
    toArray(object)->values[i] = value;
  return value;
}

bool isMap(any value) {
  return value.type() == typeid(Map*);
}

Map* toMap(any value) {
  return any_cast<Map*>(value);
}

any getValueOfMap(any object, any key) {
  if (toMap(object)->values.count(toString(key)))
    return toMap(object)->values[toString(key)];
  return nullptr;
}

any setValueOfMap(any object, any key, any value) {
  toMap(object)->values[toString(key)] = value;
  return value;
}

bool isFunction(any value) {
  return value.type() == typeid(Function*);
}

Function* toFunction(any value) {
  return any_cast<Function*>(value);
}

bool isBuiltinFunction(any value) {
  return value.type() == typeid(function<any(vector<any>)>);
}

function<any(vector<any>)> toBuiltinFunction(any value) {
  return any_cast<function<any(vector<any>)>>(value);
}

ostream& operator<<(ostream& stream, any& value) {
  if (isNull(value)) {
    stream << "null";
  }
  else if (isBoolean(value)) {
    stream << boolalpha << any_cast<bool>(value);
  }
  else if (isNumber(value)) {
    printf("%g", toNumber(value));
  }
  else if (isString(value)) {
    stream << toString(value);
  }
  else if (isArray(value)) {
    stream << "[ ";
    for (auto& value : toArray(value)->values)
      stream << value << " ";
    stream << "]";
  }
  else if (isMap(value)) {
    stream << "{ ";
    for (auto& [key, value] : toMap(value)->values)
      stream << key << ":" << value << " ";
    stream << "}";
  }
  return stream;
}