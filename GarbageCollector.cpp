#include "Machine.h"
#include "GarbageCollecter.h"

void markObject(any value) {
  if (isArray(value)) {
    if (toArray(value)->isMarked)
      return;
    toArray(value)->isMarked = true;
    for (auto& value: toArray(value)->values)
      markObject(value);
  }
  else if (isMap(value)) {
    if (toMap(value)->isMarked)
      return;
    toMap(value)->isMarked = true;
    for (auto& [key, value]: toMap(value)->values)
      markObject(value);
  }
}

void sweepObject() {
  objects.remove_if([](Object* object) {
    if (object->isMarked) {
      object->isMarked = false;
      return false;
    }
    delete object;
    return true;
  });
}

void collectGarbage() {
  for (auto& stackFrame: callStack) {
    for (auto& value: stackFrame.variables)
      markObject(value);
    for (auto& value: stackFrame.operandStack)
      markObject(value);
  }

  for (auto& [key, value]: global)
    markObject(value);

  sweepObject();
}