#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "Code.h"
#include "Node.h"

using std::get;
using std::cout;
using std::endl;
using std::setw;
using std::left;
using std::right;

using std::tuple;
using std::string;
using std::vector;
using std::map;

tuple<vector<Code>, map<string, size_t>> generate(Program*);
void printObjectCode(tuple<vector<Code>, map<string, size_t>>);
