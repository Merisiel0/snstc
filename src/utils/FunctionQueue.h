#pragma once

#include <deque>
#include <functional>

class FunctionQueue {
  std::deque<std::function<void()>> functions;

  void pushFunction(std::function<void()>&& function);

  void flush();
};