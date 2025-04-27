#pragma once
#include "Task.h"
#include "Move.h"
#include "DMove.h"
#include <map>

class ConvertSolutionTask : public Task<std::vector<DMove>> {
    private:
        String solution;
        std::array<char,4> xAxisOrder  = {'D', 'B', 'U', 'F'};
        std::array<char,4> yAxisOrder  = {'R', 'B', 'L', 'F'};
    public:
        ConvertSolutionTask(String solution): Task("Convert Solution Task", [this]{
            return func();
        }), solution(solution) {}
    private:
        std::vector<std::string> simplifySolution(String& solution);
        void applyAxisMove(std::vector<Move>& Moves, MoveType axis, int times);
    protected:
        std::vector<DMove> func();
};