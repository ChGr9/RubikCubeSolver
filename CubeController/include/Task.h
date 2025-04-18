#pragma once
#include <Arduino.h>
#include <functional>

template<typename T>
class Task {
public:
  using Func = std::function<T()>;

  Task(const String& name, Func f)
    : name(name), func(std::move(f)), status(NOT_STARTED) {}

  void execute() {
    xTaskCreatePinnedToCore(
      run, name.c_str(), 8192, this, 1, nullptr, 1
    );
  }

  T getResponse() const      { return response; }
  bool hasFinished() const   { return status == FINISHED; }

private:
  enum Status { NOT_STARTED, RUNNING, FINISHED } status;
  String   name;
  Func     func;
  T        response;

  static void run(void* inst) {
    auto* self = static_cast<Task*>(inst);
    if (self->status == NOT_STARTED) {
      self->status   = RUNNING;
      self->response = self->func();
      self->status   = FINISHED;
    }
    vTaskDelete(nullptr);
  }
};
