/*
 * scheduler.c
 *
 *  Created on: Nov 20, 2025
 *      Author: nguye
 */
#include "main.h"
#include "scheduler.h"

IWDG_HandleTypeDef hiwdg;
static uint32_t counter_for_watchdog = 0;

sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t SCH_task_count = 0;
uint8_t Error_code_G = 0;


void SCH_Init(void) {
	uint8_t i;
	for (i = 0; i < SCH_MAX_TASKS; i++)
	{
		SCH_Delete_Task(i);
	}

	Error_code_G = 0;
	SCH_task_count = 0;
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
}


uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD)
{
    uint8_t index = 0;

    if (SCH_task_count >= SCH_MAX_TASKS)
    {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return SCH_MAX_TASKS;
    }

    while (index < SCH_task_count && DELAY >= SCH_tasks_G[index].Delay)
    {
        DELAY -= SCH_tasks_G[index].Delay;
        index++;
    }

    for (uint8_t i = SCH_task_count; i > index; i--)
    {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Delay = DELAY;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].TaskID = index;
    SCH_tasks_G[index].RunMe = 0;

    if (index < SCH_task_count - 1)
    {
        SCH_tasks_G[index + 1].Delay -= DELAY;
    }

    SCH_task_count++;
    return index;
}

void SCH_Update(void)
{
    Watchdog_Refresh();
    if (SCH_task_count == 0) return;

    if (SCH_tasks_G[0].Delay > 0)
    {
        SCH_tasks_G[0].Delay--;
    }

	uint8_t i = 0;
	while (i < SCH_task_count && SCH_tasks_G[i].Delay == 0)
	{
		if (SCH_tasks_G[i].pTask)
		{
			SCH_tasks_G[i].RunMe++;
		}
		i++;
	}
}

void SCH_Dispatch_Tasks(void) {
	uint8_t Index = 0;
	if (SCH_tasks_G[Index].RunMe > 0)
	{
		sTask tmp = SCH_tasks_G[Index];

		(*SCH_tasks_G[Index].pTask)();
		SCH_tasks_G[Index].RunMe -= 1;

		SCH_Delete_Task(Index);

		if (tmp.Period > 0)
		{
			SCH_Add_Task(tmp.pTask, tmp.Period, tmp.Period);
		}
	}

	SCH_Report_Status();
	SCH_Go_To_Sleep();
}

uint8_t  SCH_Delete_Task(const uint8_t TASK_INDEX) {
	uint8_t Return_code;
	if (SCH_tasks_G[TASK_INDEX].pTask == 0)
	{
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
		Return_code = RETURN_ERROR;
	} else {
		Return_code = RETURN_NORMAL;
	}

    for (uint8_t j = TASK_INDEX; j < SCH_task_count - 1; j++)
    {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    }

    SCH_tasks_G[SCH_task_count - 1].pTask = 0;
    SCH_tasks_G[SCH_task_count - 1].Delay = 0;
    SCH_tasks_G[SCH_task_count - 1].Period = 0;
    SCH_tasks_G[SCH_task_count - 1].RunMe = 0;

	SCH_task_count--;
	return Return_code;
}


void SCH_Go_To_Sleep() {
	//todo
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}


void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
    if (Error_code_G != Last_error_code_G)
    {
        Error_port = 255 - Error_code_G;
        Last_error_code_G = Error_code_G;
        if (Error_code_G != 0) {
            Error_tick_count_G = 60000;
        } else {
            Error_tick_count_G = 0;
        }
    } else {
        if (Error_tick_count_G != 0) {
            if (--Error_tick_count_G == 0) {
                Error_code_G = 0; // Reset lỗi
            }
        }
    }
#endif
}


void Watchdog_Refresh(void) {
    HAL_IWDG_Refresh(&hiwdg);
}


uint8_t Is_Watchdog_Reset(void) {
    return (counter_for_watchdog > 3) ? 1 : 0;
}


void Watchdog_Counting(void) {
    counter_for_watchdog++;
}


void Reset_Watchdog_Counting(void) {
    counter_for_watchdog = 0;
}



