/*
 * global.h
 *
 *  Created on: Nov 20, 2025
 *      Author: nguye
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>
#include <stdbool.h>

#define BUTTON_IS_PRESSED 		GPIO_PIN_RESET
#define BUTTON_IS_RELEASED 		GPIO_PIN_SET

// Keypad 3x4 and indoor unlock button, door, key sensor
#define N0_OF_BUTTONS			15

// System states
#define INIT					0
#define LOCKED_SLEEP			1
#define LOCKED_WAKEUP			2
#define BATTERY_WARNING			3
#define LOCKED_ENTRY			4
#define LOCKED_VERIFY			5
#define PENALTY_TIMER			10
#define PERMANENT_LOCKOUT		11
#define UNLOCKED_WAITOPEN		20
#define UNLOCKED_DOOROPEN		21
#define UNLOCKED_ALWAYSOPEN		22
#define ALARM_FORGOTCLOSE		23
#define UNLOCKED_WAITCLOSE		24
#define UNLOCKED_SETPASSWORD	25
#define LOCKED_RELOCK			26

/* ------------------------- Input Processing ------------------------- */
typedef struct {
    uint8_t keypadChar;      // Last confirmed key
    uint8_t indoorButton;    // 1 = pressed, 0 = released
    uint8_t keySensor;       // 1 = turned, 0 = normal
    uint8_t doorSensor;      // 1 = open, 0 = closed
} InputState_t;

extern InputState_t gInputState;
extern char gKeyEvent;       // Last keypad event for state_processing

/* ------------------------- Output Processing ------------------------ */
typedef struct {
    uint8_t ledRed;          // 0 = off, 1 = on
    uint8_t ledGreen;        // 0 = off, 1 = on
    uint8_t buzzer;          // 0 = off, 1 = on
    uint8_t solenoid;        // 0 = locked, 1 = unlocked
    char lcdLine1[17];       // LCD line 1 (16 chars + null)
    char lcdLine2[17];       // LCD line 2
} OutputStatus_t;

extern OutputStatus_t gOutputStatus;

/* ------------------------- State Processing ------------------------- */
typedef struct {
    uint8_t currentState;    // Current system state (INIT, LOCKED_SLEEP, ...)
    uint16_t timers[10];     // Generic timers for penalties, auto-relock, etc.
} SystemState_t;

extern SystemState_t gSystemState;

/* Password storage */
#define MAX_PASSWORD_LENGTH 20
extern char gPassword[MAX_PASSWORD_LENGTH + 1];  // Null-terminated


#endif /* INC_GLOBAL_H_ */
