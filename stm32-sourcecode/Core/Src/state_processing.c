/*
 * state_processing.c
 *
 *  Created on: Nov 20, 2025
 *      Author: nguye
 */
/**
 * state_processing.c
 *
 * Implements the full system-level FSM for the electronic lock.
 *
 * See header for public API.
 */
#include "state_processing.h"
#include "global.h"
#include "output_processing.h"
#include "input_processing.h"
#include <string.h>

void State_Init(void)
{
    gSystemState.currentState = INIT;

    /* Reset timers */
    for (int i = 0; i < 10; i++) {
        gSystemState.timers[i] = 0;
    }

    /* Default password (can be empty) */
    gPassword[0] = '\0';

    /* Reset outputs */
    gOutputStatus.ledRed = 0;
    gOutputStatus.ledGreen = 0;
    gOutputStatus.buzzer = 0;
    gOutputStatus.solenoid = 0;
    memset(gOutputStatus.lcdLine1, ' ', 16);
    gOutputStatus.lcdLine1[16] = '\0';
    memset(gOutputStatus.lcdLine2, ' ', 16);
    gOutputStatus.lcdLine2[16] = '\0';
}

void State_Process(void)
{
    switch (gSystemState.currentState)
    {
        case INIT:
            gSystemState.currentState = LOCKED_SLEEP;
            break;
        case LOCKED_SLEEP:
            if (gInputState.indoorButton || gInputState.keySensor || gKeyEvent) {
                gSystemState.currentState = LOCKED_WAKEUP;
            }
            break;
        case LOCKED_WAKEUP:
            // Example: show prompt on LCD
            strncpy(gOutputStatus.lcdLine1, "Enter Password", 16);
            gSystemState.currentState = LOCKED_ENTRY;
            break;
        case LOCKED_ENTRY:
            // wait for keypad input via State_Event_KeypadChar()
            break;
        case UNLOCKED_WAITOPEN:
            // Auto-relock timer check
            if (gSystemState.timers[0] > 0) {
                gSystemState.timers[0]--;
            } else {
                gSystemState.currentState = LOCKED_RELOCK;
            }
            break;
        case LOCKED_RELOCK:
            gOutputStatus.solenoid = 0; // lock
            gSystemState.currentState = LOCKED_SLEEP;
            break;
        default:
            break;
    }
}

void State_Event_KeypadChar(char c)
{
    gKeyEvent = c;

    // Example: simple password input handling
    static char inputBuffer[21];
    static uint8_t pos = 0;

    if (c == '#') {
        // Check password
        if (strcmp(inputBuffer, gPassword) == 0) {
            gSystemState.currentState = UNLOCKED_WAITOPEN;
            gOutputStatus.solenoid = 1;
        } else {
            gSystemState.currentState = PENALTY_TIMER;
        }
        pos = 0;
        memset(inputBuffer, 0, sizeof(inputBuffer));
    } else if (c == '*') {
        // Backspace
        if (pos > 0) pos--;
        inputBuffer[pos] = '\0';
    } else {
        if (pos < MAX_PASSWORD_LENGTH) {
            inputBuffer[pos++] = c;
            inputBuffer[pos] = '\0';
        }
    }
}

void State_Event_IndoorButton(void)
{
    if (gSystemState.currentState == LOCKED_SLEEP) {
        gSystemState.currentState = LOCKED_WAKEUP;
    }
}

void State_Event_KeySensor(void)
{
    // Force unlock override
    gSystemState.currentState = UNLOCKED_WAITOPEN;
    gOutputStatus.solenoid = 1;
}

void State_Event_DoorSensor_Open(void)
{
    if (gSystemState.currentState == UNLOCKED_WAITOPEN) {
        gSystemState.currentState = UNLOCKED_DOOROPEN;
    }
}

void State_Event_DoorSensor_Close(void)
{
    if (gSystemState.currentState == UNLOCKED_DOOROPEN) {
        gSystemState.currentState = UNLOCKED_WAITCLOSE;
    }
}

bool State_SetPassword(const char *newPass)
{
    size_t len = strlen(newPass);
    if (len < 4 || len > MAX_PASSWORD_LENGTH) return false;

    strncpy(gPassword, newPass, MAX_PASSWORD_LENGTH);
    gPassword[MAX_PASSWORD_LENGTH] = '\0';
    return true;
}

const char* State_GetPassword(void)
{
    return gPassword;
}

int State_GetState(void)
{
    return gSystemState.currentState;
}

void State_ForceUnlock(void)
{
    gSystemState.currentState = UNLOCKED_WAITOPEN;
    gOutputStatus.solenoid = 1;
}






