#pragma once

class Move {
    private:
        char move;
        int times;
    public:
        Move(char move, int times) : move(move), times(times%4) {}
        char getMove() { return move; }
        int getTimes() { return times; }
        String toString() {
            String result = "";
            result += move;
            if(times > 1) {
                result += String(times);
            }
            return result;
        }
};