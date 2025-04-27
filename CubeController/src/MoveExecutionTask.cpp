#include "MoveExecutionTask.h"
#include "ServoManager.h"

bool MoveExecutionTask::func() {
    for(DMove move : moves) {
        moveIndex++;
        switch(move.getMoveType()) {
            case MoveType::X:
                for(int i = 0; i < 4- move.getTimes(); i++) {
                    ServoManager::bootKick();
                }
                break;
            case MoveType::Y:
                ServoManager::holderTurn(4 - move.getTimes());
                break;
            case MoveType::Down:
                ServoManager::handleClose();
                ServoManager::holderTurn(move.getTimes());
                ServoManager::handleOpen();
                break;
        }
    }
    return true;
}