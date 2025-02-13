#include "FunctionQueue.h"

void FunctionQueue::pushFunction(std::function<void()>&& function) {
  functions.push_back(function);
}

void FunctionQueue::flush() {
	for (auto it = functions.rbegin(); it != functions.rend(); it++) {
		(*it)();
	}

	functions.clear();
}
