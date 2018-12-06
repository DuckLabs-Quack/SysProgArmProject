#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "simpleRoundRobin.h"
#include "fixedPri.h"
#include "sleep.h"
#include "mutex.h"

static OS_mutex_t mutex;

void task1(void const *const args) {
	while (1) {
		printf("Message from Task 1\r\n");
		OS_sleep(1000);
	}
}
void task2(void const *const args) {
	while (1) {
		printf("Message from Task 2\r\n");
		OS_sleep(2000);
	}
}
void task3(void const *const args) {
	while (1) {
		printf("Message from Task 3\r\n");
		OS_sleep(3000);
	}
}
void task4(void const *const args) {
	while (1) {
		printf("Message from Task 4\r\n");
		OS_sleep(4000);
	}
}

void printTasks();

/* MAIN FUNCTION */
int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();

	printf("\r\nDocetOS Sleep and Mutex\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t stack1[64], stack2[64], stack3[64], stack4[64];
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4;

	/* Initialise the TCBs using the two functions above */
	OS_initialiseTCB(&TCB1, stack1+64, task1, 0);
	OS_initialiseTCB(&TCB2, stack2+64, task2, 0);
	OS_initialiseTCB(&TCB3, stack3+64, task3, 0);
	OS_initialiseTCB(&TCB4, stack4+64, task4, 0);
	TCB1.priority = 10;
	TCB2.priority = 6;
	TCB3.priority = 8;
	TCB4.priority = 5;
	
	// init_mutex(&mutex);

	/* Initialise and start the OS */
	//OS_init(&simpleRoundRobinScheduler);
	OS_init(&fixedPriScheduler);
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	
	printTasks();
	
	OS_start();
}
