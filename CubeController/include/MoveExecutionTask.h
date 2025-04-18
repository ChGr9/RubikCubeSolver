#pragma once
#include "Task.h"
#include "DMove.h"

class MoveExecutionTask : public Task<bool> {
    public:
        MoveExecutionTask(std::vector<DMove> moves): Task("Move Execution Task", [this]{
            return func();
        }), moves(moves), moveCount(moves.size()) {}
        int getMoveCount() { return moveCount; }
        int getMoveIndex() { return moveIndex; }
    private:
        std::vector<DMove> moves;
        int moveIndex = 0;
        int moveCount;
    protected:
        bool func();
};