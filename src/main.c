/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/*-----------------------------------------------------------*/

#include "definitions.h"

static void prvSetupHardware( void );

static dd_tasklist active_list;
static dd_tasklist completed_list;
static dd_tasklist overdue_list;

static QueueHandle_t scheduler_queue;
static QueueHandle_t monitor_queue;

/*-------------------------- Main Function ----------------------------------*/

int main(void)
{
	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */

	prvSetupHardware();
	initScheduler();

	xTaskCreate(taskGenerator1, "Task Generator 1", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_GENERATOR, &taskgen1_handle);
	xTaskCreate(taskGenerator2, "Task Generator 2", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_GENERATOR, &taskgen2_handle);
	xTaskCreate(taskGenerator3, "Task Generator 3", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_GENERATOR, &taskgen3_handle);

	vTaskStartScheduler();

	return 0;
}

/*-------------------------- Task List Code ---------------------------------*/

void initTaskList(tasklist new_list)
{
	if(new_list == NULL)
	{
		printf("initTaskList: list passed in was NULL.\n");
		return;
	}

    new_list->list_length = 0;
    new_list->list_head   = NULL;
    new_list->list_tail   = NULL;

}

task createTask()
{
	task new_task = (task)pvPortMalloc(sizeof(dd_task));

	new_task->t_handle = NULL;
	new_task->task_func = NULL;
	new_task->type = PERIODIC;
	new_task->task_id = 0;
	new_task->name = "";
	new_task->release_time = 0;
	new_task->absolute_deadline = 0;
	new_task->completion_time = 0;
	new_task->aperiodic_timer = NULL;
	new_task->next = NULL;
	new_task->prev = NULL;

	return new_task;
}

bool deleteTask(task del_task)
{
	if (del_task == NULL)
	{
		printf("deleteTask: task passed in was NULL.\n");
		return false;
	}

	// Return false if task wasn't removed from a list already
	if (del_task->next != NULL || del_task->prev != NULL)
	{
		printf("deleteTask: task needs to be removed from all lists first.\n");
		return false;
	}

	// Reset each field and free the memory
	del_task->t_handle = NULL;
	del_task->task_func = NULL;
	del_task->type = PERIODIC;
	del_task->task_id = 0;
	del_task->name = "";
	del_task->release_time = 0;
	del_task->absolute_deadline = 0;
	del_task->completion_time = 0;
	del_task->aperiodic_timer = NULL;
	del_task->next = NULL;
	del_task->prev = NULL;

	vPortFree((void*)del_task);

	return true;
}

char* taskListReturnMessages(tasklist cur_list)
{
	// Get the size of the list and create a buffer based on that size
	uint32_t list_size = cur_list->list_length;
	uint32_t buf_size = ( (configMAX_TASK_NAME_LEN + 50) * (list_size + 1)) ;
	char* output_buf = (char*)pvPortMalloc(buf_size);
	output_buf[0] = '\0';

	if (list_size == 0)
	{
		char buf[20] = ("List is empty.\n");
		strcat(output_buf, buf);
	}
	else {
		task cur_task = cur_list->list_head;
		while (cur_task != NULL)
		{
			char cur_buf[70];
			sprintf(cur_buf, "Task ID = %lu, Deadline = %u \n", cur_task->task_id, (unsigned int)cur_task->absolute_deadline );
			strcat(output_buf, cur_buf);

			cur_task = cur_task->next;
		}
	}

	return output_buf;
}

void taskListInsert(task new_task, tasklist list)
{
	if ((new_task == NULL) || (list == NULL))
	{
		printf("taskListInsert: task or list passed in was NULL.\n");
		return;
	}

	// If the list is empty we can simply add the new task and return
	if (list->list_length == 0)
	{
		list->list_length = 1;
		list->list_head = new_task;
		list->list_tail = new_task;
		vTaskPrioritySet(new_task->t_handle, DD_TASK_PRIORITY_EXECUTION_BASE);
		return;
	}

	// Set the current task to the head of the list and store its priority (current highest priority)
	task cur_task = list->list_head;
	uint32_t cur_priority = uxTaskPriorityGet(cur_task->t_handle);

	if(cur_priority == DD_TASK_PRIORITY_GENERATOR)
	{
		printf("taskListInsert: cannot schedule any more tasks.\n");
		return;
	}

	cur_priority++;

	// Search for the proper location to add the new task based on deadlines
	while (cur_task != NULL)
	{
		if (new_task->absolute_deadline < cur_task->absolute_deadline)
		{
			if (cur_task == list->list_head)
			{
				list->list_head = new_task;
			}

			new_task->next = cur_task;
			new_task->prev = cur_task->prev;
			cur_task->prev = new_task;

			(list->list_length)++;

			vTaskPrioritySet(new_task->t_handle, cur_priority);
			return;
		}
		else
		{
			// If we have reached the end of the list, insert the new task here
			if (cur_task->next == NULL)
			{
				new_task->next = NULL;
				new_task->prev = cur_task;
				cur_task->next = new_task;
				list->list_tail = new_task;

				(list->list_length)++;
				vTaskPrioritySet(cur_task->t_handle, cur_priority );
				vTaskPrioritySet(new_task->t_handle, DD_TASK_PRIORITY_EXECUTION_BASE);
				return;
			}

			vTaskPrioritySet(cur_task->t_handle, cur_priority );
			cur_priority--;
			cur_task = cur_task->next;
		}
	}
}

void taskListRemoveFront(tasklist rem_list)
{
	if (rem_list == NULL)
	{
		printf("taskListRemoveFront: list passed in was NULL.\n");
		return;
	}

	if (rem_list->list_length == 0)
	{
		printf("taskListRemoveFront: list is already empty.\n");
		return;
	}

	task head = rem_list->list_head;

	if (rem_list->list_length == 1)
	{
		// Remove the only task in the list
		rem_list->list_head = NULL;
		rem_list->list_tail = NULL;
	}
	else
	{
		// Remove the head of the list
		rem_list->list_head = head->next;
		head->next->prev = NULL;
	}

	// Decrement the list size and delete the task from memory
	(rem_list->list_length)--;
	head->next = NULL;
	head->prev = NULL;
	deleteTask(head);
}

void taskListRemove(task rem_task, tasklist rem_list, bool clear)
{
	if ((rem_task == NULL) || (rem_list == NULL))
	{
		printf("taskListRemove: task or list passed in was NULL.\n");
		return;
	}

	if (rem_list->list_length == 0)
	{
		printf("taskListRemove: list is already empty.\n");
		return;
	}

	// Set the current task to the head of the list and store its priority (current highest priority)
	task cur_task = rem_list->list_head;
	uint32_t cur_priority = uxTaskPriorityGet(cur_task->t_handle);

	while (cur_task != NULL)
	{
		// Found the task we want to delete
		if (cur_task->t_handle == rem_task->t_handle)
		{
			// If the task is aperiodic we need to stop the timer first
			if (cur_task->type == APERIODIC && cur_task->aperiodic_timer != NULL)
			{
				xTimerStop(cur_task->aperiodic_timer, 0);
				xTimerDelete(cur_task->aperiodic_timer, 0);
				cur_task->aperiodic_timer = NULL;
			}

			if (rem_task->t_handle == rem_list->list_head->t_handle)
			{
				// Remove the head of the list
				rem_list->list_head = cur_task->next;
				cur_task->next->prev = NULL;
			}
			else if (rem_task->t_handle == rem_list->list_tail->t_handle)
			{
				// Remove the tail of the list
				rem_list->list_tail = cur_task->prev;
				cur_task->prev->next = NULL;
			}
			else if (rem_list->list_length == 1)
			{
				// Remove the only item in the list
				rem_list->list_head = NULL;
				rem_list->list_tail = NULL;
			}
			else
			{
				cur_task->prev->next = cur_task->next;
				cur_task->next->prev = cur_task->prev;
			}

			// Decrement the list size and delete the task from memory if clear is true
			(rem_list->list_length)--;
			cur_task->next = NULL;
			cur_task->prev = NULL;
			if (clear) deleteTask(cur_task);

			return;
		}

		cur_priority--;
		vTaskPrioritySet(cur_task->t_handle, cur_priority);
		cur_task = cur_task->next;
	}

	cur_task = rem_list->list_tail;
	cur_priority = DD_TASK_PRIORITY_EXECUTION_BASE;
	vTaskPrioritySet(cur_task->t_handle, cur_priority);

	while (cur_task->prev != NULL)
	{
		cur_priority++;
		cur_task = cur_task->prev;
		vTaskPrioritySet(cur_task->t_handle, cur_priority);
	}
}

void taskListCleanup(tasklist active_list, tasklist overdue_list)
{
	if ((active_list == NULL) || (overdue_list == NULL))
	{
		printf("taskListCleanup: at least one of the lists passed in was NULL.\n");
		return;
	}

	task cur_task = active_list->list_head;
	TickType_t cur_time = xTaskGetTickCount();

	while (cur_task != NULL)
	{
		if (cur_task->absolute_deadline < cur_time)
		{
			taskListRemove(cur_task->t_handle, active_list, false);

			if (overdue_list->list_length != 0)
			{
				// Add the overdue task to the end of the overdue list
				task temp = overdue_list->list_tail;
				overdue_list->list_tail = cur_task;
				temp->next = cur_task;
				cur_task->prev = temp;
				(overdue_list->list_length)++;
			}
			else
			{
				// Add the overdue task as the only element in the overdue list
				overdue_list->list_length = 1;
				overdue_list->list_head = cur_task;
				overdue_list->list_tail = cur_task;
			}

			if (cur_task->type == PERIODIC)
			{
				vTaskSuspend(cur_task->t_handle);
				vTaskDelete(cur_task->t_handle);
			}
		}
		else
		{
			return;
		}

		cur_task = cur_task->next;
	}
}

/*-------------------------- DD Scheduler Code ------------------------------*/

void initScheduler(void)
{
	initTaskList(&active_list);
	//initTaskList(&completed_list);
	initTaskList(&overdue_list);

	scheduler_queue = xQueueCreate(DD_TASK_RANGE, sizeof(dd_message));
	monitor_queue = xQueueCreate(2, sizeof(dd_message));

	vQueueAddToRegistry(scheduler_queue, "Scheduler Queue");
	vQueueAddToRegistry(monitor_queue, "Monitor Queue");

	xTaskCreate(schedulerTask, "DD Scheduler Task", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_SCHEDULER, NULL);
	xTaskCreate(monitorTask, "Monitor Task", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_MONITOR, NULL);
}

void schedulerTask(void *pvParameters)
{
	dd_message msg;
	task cur_task = NULL;

	while (1)
	{
		// Wait until a message is on the scheduler queue
		if (xQueueReceive(scheduler_queue, (void*)&msg, portMAX_DELAY) == pdTRUE)
		{
			taskListCleanup(&active_list, &overdue_list);

			while (overdue_list.list_length > 4)
			{
				taskListRemoveFront(&overdue_list);
			}

			if (msg.message_type == CREATE)
			{
				// Add the task to the active list since it has been created
				cur_task = (task)msg.message_data;
				taskListInsert(cur_task, &active_list);

				// If aperiodic, we also need to start a timer
				if (cur_task->type == APERIODIC)
				{
					TickType_t period = cur_task->absolute_deadline - xTaskGetTickCount();
					cur_task->aperiodic_timer = xTimerCreate("aperiodic_timer", period, pdFALSE, (void*)cur_task, apTimerCallback);
					xTimerStart(cur_task->aperiodic_timer, 0);
				}

				xTaskNotifyGive(msg.message_sender);
			}
			else if (msg.message_type == DELETE)
			{
				// Remove the task from the active list and add it to the completed list
				taskListRemove(msg.message_sender, &active_list, false);
				//taskListInsert(msg.message_sender, &completed_list);

				xTaskNotifyGive(msg.message_sender);
			}
			else if (msg.message_type == ACTIVE)
			{
				msg.message_data = (void*)taskListReturnMessages(&active_list);

				if(uxQueueSpacesAvailable(monitor_queue) == 0) xQueueReset(monitor_queue);

				if (monitor_queue != NULL)
				{
					if (xQueueSend(monitor_queue, &msg, (TickType_t)portMAX_DELAY) != pdPASS)
					{
						printf("Error: Can't send on Monitor Queue.\n");
						return;
					}
				}
				else
				{
					printf("Error: Monitor Queue has not been created.\n");
					return;
				}
			}
//			else if (msg.message_type == COMPLETED)
//			{
//				msg.message_data = (void*)taskListReturnMessages(&completed_list);
//				if(uxQueueSpacesAvailable(monitor_queue) == 0) xQueueReset(monitor_queue);
//
//				if (monitor_queue != NULL)
//				{
//					if (xQueueSend(monitor_queue, &msg, (TickType_t)portMAX_DELAY) != pdPASS)
//					{
//						printf("Error: Can't send on Monitor Queue.\n");
//						return;
//					}
//				}
//				else
//				{
//					printf("Error: Monitor Queue has not been created.\n");
//					return;
//				}
//			}
			else if (msg.message_type == OVERDUE)
			{
				msg.message_data = (void*)taskListReturnMessages(&completed_list);
				if(uxQueueSpacesAvailable(monitor_queue) == 0) xQueueReset(monitor_queue);

				if (monitor_queue != NULL)
				{
					if (xQueueSend(monitor_queue, &msg, (TickType_t)portMAX_DELAY) != pdPASS)
					{
						printf("Error: Can't send on Monitor Queue.\n");
						return;
					}
				}
				else
				{
					printf("Error: Monitor Queue has not been created.\n");
					return;
				}
			}
		}
	}
}

bool createDDTask(task new_task)
{
	if (new_task == NULL)
	{
		printf("createDDTask: task passed in was NULL.\n");
		return false;
	}

	xTaskCreate(new_task->task_func, new_task->name, configMINIMAL_STACK_SIZE, (void*)new_task, DD_TASK_PRIORITY_MINIMUM, &(new_task->t_handle));

	if (new_task->t_handle == NULL)
	{
		printf("createDDTask: failed to create new task.\n");
		return false;
	}

	// Suspend until the new task has been scheduled
	vTaskSuspend(new_task->t_handle);

	dd_message create_msg = {CREATE, xTaskGetCurrentTaskHandle(), new_task};

	if (scheduler_queue != NULL)
	{
		if (xQueueSend(scheduler_queue, &create_msg, portMAX_DELAY) != pdPASS)
		{
			printf("createDDTask: could not send on scheduler queue.\n");
			return false;
		}
	}
	else
	{
		printf("createDDTask: scheduler queue does not exist.\n");
		return false;
	}

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	vTaskResume(new_task->t_handle);
	return true;
}

bool deleteDDTask(task del_task)
{
	if (del_task == NULL)
	{
		printf("deleteDDTask: task passed in was NULL.\n");
		return false;
	}

	dd_message delete_msg = {DELETE, del_task, NULL};

	if (scheduler_queue != NULL)
	{
		if (xQueueSend(scheduler_queue, &delete_msg, portMAX_DELAY) != pdPASS)
		{
			printf("deleteDDTask: could not send on scheduler queue.\n");
			return false;
		}
	}
	else
	{
		printf("deleteDDTask: scheduler queue does not exist.\n");
		return false;
	}

	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	vTaskDelete(del_task);
	return true;
}

bool getActiveDDTaskList(void)
{
	dd_message active_msg = {ACTIVE, NULL, NULL};

	if (scheduler_queue != NULL)
	{
		if (xQueueSend(scheduler_queue, &active_msg, portMAX_DELAY) != pdPASS)
		{
			printf("getActiveDDTaskList: could not send on scheduler queue.\n");
			return false;
		}
	}
	else
	{
		printf("getActiveDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	if (scheduler_queue != NULL)
	{
		if (xQueueReceive(scheduler_queue, &active_msg, portMAX_DELAY) == pdTRUE)
		{
			printf("Active Task List: \n%s\n", (char*)(active_msg.message_data));
			vPortFree(active_msg.message_data);
			active_msg.message_data = NULL;
		}
	}
	else
	{
		printf("getActiveDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	return true;
}

bool getCompletedDDTaskList(void)
{
	dd_message completed_msg = {ACTIVE, NULL, NULL};

	if (scheduler_queue != NULL)
	{
		if (xQueueSend(scheduler_queue, &completed_msg, portMAX_DELAY) != pdPASS)
		{
			printf("getCompletedDDTaskList: could not send on scheduler queue.\n");
			return false;
		}
	}
	else
	{
		printf("getCompletedDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	if (scheduler_queue != NULL)
	{
		if (xQueueReceive(scheduler_queue, &completed_msg, portMAX_DELAY) == pdTRUE)
		{
			printf("Completed Task List: \n%s\n", (char*)(completed_msg.message_data));
			vPortFree(completed_msg.message_data);
			completed_msg.message_data = NULL;
		}
	}
	else
	{
		printf("getCompletedDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	return true;
}

bool getOverdueDDTaskList(void)
{
	dd_message overdue_msg = {ACTIVE, NULL, NULL};

	if (scheduler_queue != NULL)
	{
		if (xQueueSend(scheduler_queue, &overdue_msg, portMAX_DELAY) != pdPASS)
		{
			printf("getOverdueDDTaskList: could not send on scheduler queue.\n");
			return false;
		}
	}
	else
	{
		printf("getOverdueDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	if (scheduler_queue != NULL)
	{
		if (xQueueReceive(scheduler_queue, &overdue_msg, portMAX_DELAY) == pdTRUE)
		{
			printf("Overdue Task List: \n%s\n", (char*)(overdue_msg.message_data));
			vPortFree(overdue_msg.message_data);
			overdue_msg.message_data = NULL;
		}
	}
	else
	{
		printf("getOverdueDDTaskList: scheduler queue does not exist.\n");
		return false;
	}

	return true;
}

static void apTimerCallback(xTimerHandle xTimer)
{
	task ap_task = (task)pvTimerGetTimerID(xTimer);

	xTimerDelete(ap_task->aperiodic_timer, 0);
	ap_task->aperiodic_timer = NULL;

	vTaskSuspend(ap_task->t_handle);
	vTaskDelete(ap_task->t_handle);
}

/*-------------------------- Monitor Task Code ------------------------------*/

void monitorTask ( void *pvParameters )
{
	vTaskDelay(10000);

    while(1)
    {
    	printf("\nMonitorTask: Current Time = %u, Priority = %u\n", (unsigned int)xTaskGetTickCount(), (unsigned int)uxTaskPriorityGet(NULL));
        getActiveDDTaskList();
        getCompletedDDTaskList();
        getOverdueDDTaskList();
        vTaskDelay(100);
    }
}

/*-------------------------- FreeRTOS Hooks ---------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

/*-------------------------- Hardware Setup ---------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );
}
