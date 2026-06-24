# STM32F103C8 Blue Pill - FreeRTOS LED Blinking (3 Independent Tasks)

## Overview

This project demonstrates the basic usage of FreeRTOS on the STM32F103C8 Blue Pill development board. The objective is to understand how a Real-Time Operating System (RTOS) schedules multiple tasks independently.

Three separate LED blinking tasks are created, each running at a different delay interval while sharing the same priority level.

This is the first step toward learning RTOS concepts such as task creation, task scheduling, delays, and multitasking.



## Project Objective

Create three independent FreeRTOS tasks:

| Task   | Delay   |
| ------ | ------- |
| Task 1 | 200 ms  |
| Task 2 | 500 ms  |
| Task 3 | 1000 ms |

All tasks are assigned equal priority.

The scheduler switches between tasks automatically, allowing all LEDs to blink independently.



## RTOS Concepts Learned

### 1. Task Creation

Each LED is controlled by a separate FreeRTOS task using:
xTaskCreate()

### 2. Task Scheduling

After starting the scheduler:
vTaskStartScheduler();
FreeRTOS manages CPU time between all tasks.

### 3. Task Delay
Each task enters the Blocked state using:
vTaskDelay()
This allows other tasks to execute while the current task waits.

### 4. Equal Priority Tasks
All tasks are created with the same priority:
Since priorities are equal, FreeRTOS schedules tasks fairly.

## FreeRTOS Task States Observed

The tasks continuously move between:

### Running

Task currently using the CPU.

### Ready

Task waiting for CPU time.

### Blocked

Task waiting inside `vTaskDelay()`.


