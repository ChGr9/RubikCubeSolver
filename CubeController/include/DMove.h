#pragma once
#include "MoveType.h"
#include <Arduino.h>

class DMove {
    private:
        MoveType moveType;
        int times;
    public:
        DMove(MoveType moveType, int times) : moveType(moveType), times(times%4) {}
        MoveType getMoveType() { return moveType; }
        int getTimes() { return times; }
        String toString() {
            String result = "";
            switch(moveType) {
                case MoveType::Down:
                    result = "D";
                    break;
                case MoveType::X:
                    result = "X";
                    break;
                case MoveType::Y:
                    result = "Y";
                    break;
                default:
                    result = "Invalid";
                    break;
            }
            if(times > 1) {
                result += String(times);
            }
            return result;
        }
};