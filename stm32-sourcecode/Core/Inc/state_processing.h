/*
 * state_processing.h
 *
 *  Created on: Nov 20, 2025
 *      Author: nguye
 */

#ifndef INC_STATE_PROCESSING_H_
#define INC_STATE_PROCESSING_H_

/**
 * @file state_processing.h
 * @brief State machine module
 *
 * Notes:
 *  - All global variables (e.g., gSystemState, gSystemTimers) are declared in global.h
 *  - Input events come from input_processing (via gKeyEvent or State_Event_* calls)
 *  - Output updates via gOutputStatus
 */

#ifndef STATE_PROCESSING_H_
#define STATE_PROCESSING_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize state processing module.
 *
 * Call once in main after Input_Init() and Output_Init().
 */
void State_Init(void);

/**
 * @brief Periodic state processing routine.
 *
 * Recommended to call every 10ms from scheduler.
 * Handles timers, penalties, auto-relock, and recurring alarms.
 */
void State_Process(void);

/**
 * @brief Handle confirmed keypad character.
 * @param c Character pressed ('0'..'9', 'A'..'F', '*', '#')
 */
void State_Event_KeypadChar(char c);

/**
 * @brief Handle indoor unlock button pressed
 */
void State_Event_IndoorButton(void);

/**
 * @brief Handle mechanical key sensor triggered
 */
void State_Event_KeySensor(void);

/**
 * @brief Handle door opened
 */
void State_Event_DoorSensor_Open(void);

/**
 * @brief Handle door closed
 */
void State_Event_DoorSensor_Close(void);

/**
 * @brief Set new password
 * @param newPass Null-terminated string, 4..20 chars
 * @return true if accepted
 */
bool State_SetPassword(const char *newPass);

/**
 * @brief Get current password
 * @return Pointer to password string
 */
const char* State_GetPassword(void);

/**
 * @brief Get current system state
 */
int State_GetState(void);

/**
 * @brief Force permanent unlock (debug / override)
 */
void State_ForceUnlock(void);

#endif /* INC_STATE_PROCESSING_H_ */

