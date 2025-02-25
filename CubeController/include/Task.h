#pragma once
#include <Arduino.h>

template<typename T>
class Task {
public:
    enum Status {
        NOT_STARTED,
        RUNNING,
        FINISHED
    };

    Task(const String name): name(name), status(NOT_STARTED) {}

    virtual ~Task() {}

    void execute() {
        xTaskCreatePinnedToCore(
            run,
            name.c_str(),
            8192,
            this,
            1,
            NULL,
            1
        );
    }

    T getResponse() const { return response; }
    bool hasFinished() const { return status == FINISHED; }

protected:
    virtual T func() = 0;

private:
    T response;
    Status status;
    const String name;
    static void run(void* instance) {
        auto* task = static_cast<Task<T>*>(instance);
        if (task->status == NOT_STARTED) {
            task->status = RUNNING;
            task->response = task->func();
            task->status = FINISHED;
        }
        vTaskDelete(nullptr);
    }
};