#include <iomanip>
#include "Kind.h"

using std::string;

struct Token {
  Kind kind = Kind::Unknown;
  string string;
};
