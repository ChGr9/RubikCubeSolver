#include "ServoManager.h"

Servo ServoManager::bootServo;
Servo ServoManager::handleServo;
Servo ServoManager::holderServo;
bool ServoManager::isBootUp;
int ServoManager::holderPosIndex;

void ServoManager::init() {
    isBootUp = false;
    holderPosIndex = 0;
    ESP32PWM::allocateTimer(0);
    bootServo.setPeriodHertz(50);
    bootServo.attach(BOOT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
    handleServo.setPeriodHertz(50);
    handleServo.attach(HANDLE_SERVO_PIN, SERVO_MIN, SERVO_MAX);
    holderServo.setPeriodHertz(50);
    holderServo.attach(HOLDER_SERVO_PIN, SERVO_MIN, 2540);
    resetServos();
}

void ServoManager::lowerBoot() {
    isBootUp = false;
    bootServo.writeMicroseconds(bootRestPos);
    delay(1000);
}

void ServoManager::bootKick() {
    if(isBootUp) {
        lowerBoot();
    }
    bootServo.writeMicroseconds(bootKickPos);
    delay(400);
    bootServo.writeMicroseconds(bootMidPos);
    delay(600);
    bootServo.writeMicroseconds(bootKickPos);
    delay(400);
    bootServo.writeMicroseconds(bootRestPos);
    delay(1000);
}

void ServoManager::bootHalfKick() {
    if(isBootUp) {
        lowerBoot();
    }
    bootServo.writeMicroseconds(bootKickPos);
    delay(400);
    bootServo.writeMicroseconds(bootMidPos);
    delay(600);
    bootServo.writeMicroseconds(bootKickPos);
    delay(1000);
    isBootUp = true;
}

void ServoManager::handleOpen() {
    if(isBootUp) {
        lowerBoot();
    }
    handleServo.writeMicroseconds(handleOpenPos);
    delay(1000);
}

void ServoManager::handleClose() {
    if(isBootUp) {
        lowerBoot();
    }
    handleServo.writeMicroseconds(handleClosePos);
    delay(1000);
}

void ServoManager::holderTurn(int times) {
    if(isBootUp) {
        lowerBoot();
    }
    holderPosIndex += times;
    holderPosIndex %= 4;
    holderServo.writeMicroseconds(holderPos[holderPosIndex]);
    delay(1500);
}

void ServoManager::resetServos() {
    bootServo.writeMicroseconds(bootRestPos);
    handleServo.writeMicroseconds(handleOpenPos);
    delay(1000);
    holderPosIndex = 0;
    holderServo.writeMicroseconds(holderPos[0]);
    delay(500);
}