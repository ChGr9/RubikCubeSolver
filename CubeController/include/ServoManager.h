#pragma once
#include "ESP32SERVO_WRAPPER.h"

#define SERVO_MIN 500
#define SERVO_MAX 2500

class ServoManager {
    private:
        static Servo bootServo;
        #define BOOT_SERVO_PIN 27
        static const int bootRestPos = 1166;
        static const int bootMidPos = 1050;
        static const int bootKickPos = 940;

        static Servo handleServo;
        #define HANDLE_SERVO_PIN 12
        static const int handleOpenPos = 1166;
        static const int handleClosePos = 1920;

        static Servo holderServo;
        #define HOLDER_SERVO_PIN 25

        static bool isBootUp;
        inline static int holderPos[4] = {500, 1200, 1930, 2540};
        static int holderPosIndex;

        static void lowerBoot();

    public:
        static void init();
        static void bootKick();
        static void bootHalfKick();
        static void handleOpen();
        static void handleClose();
        static void holderTurn(int times);
        static void resetServos();
};