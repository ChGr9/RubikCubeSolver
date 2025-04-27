#include "ConvertSolutionTask.h"
#include "StringUtils.h"

std::vector<DMove> ConvertSolutionTask::func() {
    std::vector<Move> moves;
    for(std::string moveStr : simplifySolution(solution)){
        char m = moveStr.front();
        int times = 1;
        if(moveStr.size() > 1) {
            if(moveStr[1] == '2') {
                times = 2;
            }
            else if(moveStr[1] == 'i') {
                times = 3;
            }
        }
        moves.push_back(Move(m, times));
    }
    std::vector<DMove> dMoves;
    while(!moves.empty()) {
        Move move = moves.front();
        moves.erase(moves.begin());
        switch (move.getMove())
        {
            case 'F':
                dMoves.push_back(DMove(MoveType::X, 3));
                applyAxisMove(moves, MoveType::X, 3);
                break;
            case 'B':
                dMoves.push_back(DMove(MoveType::Y, 2));
                applyAxisMove(moves, MoveType::Y, 2);
                dMoves.push_back(DMove(MoveType::X, 3));
                applyAxisMove(moves, MoveType::X, 3);
                break;
            case 'R':
                dMoves.push_back(DMove(MoveType::Y, 1));
                applyAxisMove(moves, MoveType::Y, 1);
                dMoves.push_back(DMove(MoveType::X, 3));
                applyAxisMove(moves, MoveType::X, 3);
                break;
            case 'L':
                dMoves.push_back(DMove(MoveType::Y, 3));
                applyAxisMove(moves, MoveType::Y, 3);
                dMoves.push_back(DMove(MoveType::X, 3));
                applyAxisMove(moves, MoveType::X, 3);
                break;
            case 'U':
                dMoves.push_back(DMove(MoveType::X, 2));
                applyAxisMove(moves, MoveType::X, 2);
                break;
            case 'D':
                break;
        }
        dMoves.push_back(DMove(MoveType::Down, move.getTimes()));
    }
    return dMoves;
}

void ConvertSolutionTask::applyAxisMove(std::vector<Move>& Moves, MoveType axis, int times){
    std::array<char,4> order;
    switch(axis) {
        case MoveType::X:
            order = xAxisOrder;
            break;
        case MoveType::Y:
            order = yAxisOrder;
            break;
    }
    
    for(int i=0; i<Moves.size(); i++) {
        Move move = Moves[i];
        for(int j=0; j<4; j++) {
            if(order[j] == move.getMove()) {
                Moves[i] = Move(order[(j+times)%4], move.getTimes());
                break;
            }
        }
    }
}

std::vector<std::string> ConvertSolutionTask::simplifySolution(String& solution) {
    std::vector<std::string> solutionList = StringUtils::split(solution.c_str(), ',');
    std::vector<std::string> result;
    for(int i=0; i<solutionList.size(); i++) {
        std::string move = StringUtils::trim(solutionList[i]);
        if(move.find('s') != std::string::npos) {
            char m = move.front();
            char opposite = m == 'R' ? 'L' : m == 'L' ? 'R' : m == 'U' ? 'D' : m == 'D' ? 'U' : m == 'F' ? 'B' : 'F';
            bool twice = move.back() == '2';
            bool inverse = move.back() == 'i';
            result.push_back(std::string(1, opposite) + (twice ? "2" : (inverse ? "" : "i")));
            result.push_back(std::string(1, m) + (twice ? "2" : (inverse ? "i" : "")));
        }
        else if (move.find('a') != std::string::npos) {
            char m = move.front();
            char opposite = m == 'R' ? 'L' : m == 'L' ? 'R' : m == 'U' ? 'D' : m == 'D' ? 'U' : m == 'F' ? 'B' : 'F';
            bool twice = move.back() == '2';
            bool inverse = move.back() == 'i';
            result.push_back(std::string(1, opposite) + (twice ? "2" : (inverse ? "i" : "")));
            result.push_back(std::string(1, m) + (twice ? "2" : (inverse ? "i" : "")));
        }
        else {
            result.push_back(move);
        } 
    }
    return result;
}