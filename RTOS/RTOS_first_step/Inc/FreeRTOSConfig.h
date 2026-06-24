#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                    1
#define configCPU_CLOCK_HZ                      8000000UL
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    5
#define configTOTAL_HEAP_SIZE                   ( 6 * 1024 )
#define configMINIMAL_STACK_SIZE                128
#define configTIMER_TASK_STACK_DEPTH            128
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_TICK_HOOK                     0
#define configUSE_IDLE_HOOK                     0
#define configUSE_MUTEXES                       1
#define configUSE_SEMAPHORES                    1
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                5

#define configUSE_16_BIT_TICKS                  0
#define configCHECK_HANDLER_INSTALLATION        0

#define configPRIO_BITS                         4
#define configKERNEL_INTERRUPT_PRIORITY         ( 15 << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 5  << (8 - configPRIO_BITS) )

#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1

#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif
