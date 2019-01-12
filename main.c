#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "simpleRoundRobin.h"
#include "fixedPri.h"
#include "sleep.h"
#include "mutex.h"
#include "circularBuffer.h"
#include "memPool.h"
#include "interrupt.h"

#define ENTER(x) printf("ENTER "#x"\r\n")
#define MID(x) printf("MID "#x"\r\n")
#define EXIT(x) printf("EXIT "#x"\r\n")


//#define ENTER(x) 
//#define MID(x) 
//#define EXIT(x) 

static OS_mutex_t mutex;
static queue_t queue;

void printStackPointers();

void task1(void const *const args) {
	float value;
	int status;
	while (1) {
		ENTER(1);
		OS_mutex_acquire(&mutex);
		MID(1);
		
		status = queue_get(&queue, &value);
		printf("Message from Task 1: Status: %d Value: %0.1f \r\n", status, value);
		
		status=queue_get(&queue, &value);
		printf("Message from Task 1: Status: %d Value: %0.1f \r\n", status, value);
		
		status=queue_get(&queue, &value);
		printf("Message from Task 1: Status: %d Value: %0.1f \r\n", status, value);
		OS_sleep(500);
		OS_mutex_release(&mutex);
		EXIT(1);
		OS_sleep(1000);
		
	}
}
void task2(void const *const args) {
	int value;
	int status;
	while (1) {
		ENTER(2);
		OS_mutex_acquire(&mutex);
		MID(2);
		printStackPointers();
		printf("Message from Task 2\r\n");
		OS_sleep(3000);
		OS_mutex_release(&mutex);
		EXIT(2);
		OS_sleep(3000);
	}
}
void task3(void const *const args) {
	while (1) {
		ENTER(3);
		OS_mutex_acquire(&mutex);
		MID(3);
		printf("Message from Task 3\r\n");
		OS_sleep(3000);
		OS_mutex_release(&mutex);
		EXIT(3);
		OS_sleep(4000);
		
	}
}
void task4(void const *const args) {
	while (1) {
		ENTER(4);
		OS_mutex_acquire(&mutex);
		MID(4);
		printf("Message from Task 4\r\n");
		MID(4a);
		queue_put(&queue, 1.5f);
		queue_put(&queue, 2.4f);
		queue_put(&queue, 3.6f);
		OS_sleep(2000);
		MID(4b);
		OS_mutex_release(&mutex);
		EXIT(4);
		interrupt_test(&mutex);
		//OS_ISR_notify(&mutex.waitlist);
		OS_sleep(5000);
	}
}

void task5(void const *const args) {
	float f = 20.1f;
	while (1) {
		ENTER(5);
		OS_mutex_acquire(&mutex);
		MID(5);
		printStackPointers();
		printf("Message from Task 5: %.1f\r\n", f);
		MID(5a);
		//printf("Message from Task 5: %u\r\n", (int)f);
		f = 20.1f;
		OS_sleep(2000);
		OS_mutex_release(&mutex);
		EXIT(5);
		OS_sleep(4000);
	}
}

OS_TCB_t* orderedTasks[10];

uint32_t taskID(OS_TCB_t* task) {
	for (int i = 0; i < FIX_PRIO_MAX_TASKS; i++) {
		if (orderedTasks[i] == task) {
			return i+1;
		}
	}
	return -1;
}

uint32_t getCurrentTaskID() {
	return taskID(OS_currentTCB());
}

void printStackPointers() {
	for (int i = 0; i < 5; i++) {
		printf("%#010x\r\n", (uint32_t)orderedTasks[i]->sp);
	}
}

void printTasks();

/* MAIN FUNCTION */
int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();
	mutex_init(&mutex);
	queue_init(&queue);
	
	float f = 2.0f;
	
	for (int i=0;i<10;i++) {
		f *= (float)i;
		printf("%.1f\r\n", (float)f);
	}
	
	int item1 = 1;
	int item2 = 2;
	int item3 = 3;
	
	linked_list_t testList;
	linked_list_init(&testList);
	linked_list_add(&testList, &item1);
	linked_list_add(&testList, &item2);
	linked_list_add(&testList, &item3);
	
	for (int i = 0; i<3; i++) {
		int* testItem = linked_list_remove(&testList);
		printf("\r\n Test: %d \r\n", *testItem);
	}
	
	printf("\r\nDocetOS Sleep and Mutex\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t stack1[256], stack2[256], stack3[256], stack4[256], stack5[256];
	//static uint32_t stack1[1024], stack2[1024], stack3[1024], stack4[1024], stack5[1024];
	static OS_TCB_t TCB1, TCB2, TCB3, TCB4, TCB5;
	
	// DEBUG
	uint32_t i = 0;
	orderedTasks[i++] = &TCB1;
	orderedTasks[i++] = &TCB2;
	orderedTasks[i++] = &TCB3;
	orderedTasks[i++] = &TCB4;
	orderedTasks[i++] = &TCB5;
	
	// END DEBUG
	
	FPU->FPCCR &= FPU_FPCCR_ASPEN_Msk;
	FPU->FPCAR;
	SCB->CPACR |= 0xf << 20;
	//SCB->CPACR |= 0b00000000000100000000000000000000 << 20;

	/* Initialise the TCBs using the two functions above */
	//OS_initialiseTCB(&TCB1, stack1+64, task1, 0);
	//OS_initialiseTCB(&TCB2, stack2+64, task2, 0);
	//OS_initialiseTCB(&TCB3, stack3+64, task3, 0);
	//OS_initialiseTCB(&TCB4, stack4+64, task5, 0);
	OS_initialiseTCB(&TCB1, stack1+256, task1, 0);
	OS_initialiseTCB(&TCB2, stack2+256, task2, 0);
	OS_initialiseTCB(&TCB3, stack3+256, task3, 0);
	OS_initialiseTCB(&TCB4, stack4+256, task4, 0);
	OS_initialiseTCB(&TCB5, stack5+256, task5, 0);
	//OS_initialiseTCB(&TCB1, stack1+1024, task1, 0);
	//OS_initialiseTCB(&TCB2, stack2+1024, task2, 0);
	//OS_initialiseTCB(&TCB3, stack3+1024, task3, 0);
	//OS_initialiseTCB(&TCB4, stack4+1024, task4, 0);
	//OS_initialiseTCB(&TCB5, stack5+1024, task5, 0);
	TCB1.priority = 4;
	TCB2.priority = 2;
	TCB3.priority = 3;
	TCB4.priority = 1;
	TCB5.priority = 0;

	/* Initialise and start the OS */
	//OS_init(&simpleRoundRobinScheduler);
	OS_init(&fixedPriScheduler);
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	OS_addTask(&TCB4);
	OS_addTask(&TCB5);
	
	printTasks();
	
	printf("TOP\r\n");
	printf("%#010x\r\n", (uint32_t)stack1);
	printf("%#010x\r\n", (uint32_t)stack2);
	printf("%#010x\r\n", (uint32_t)stack3);
	printf("%#010x\r\n", (uint32_t)stack4);
	printf("%#010x\r\n", (uint32_t)stack5);
	printf("BOTTOM\r\n");
	printf("%#010x\r\n", (uint32_t)stack1+256*4);
	printf("%#010x\r\n", (uint32_t)stack2+256*4);
	printf("%#010x\r\n", (uint32_t)stack3+256*4);
	printf("%#010x\r\n", (uint32_t)stack4+256*4);
	printf("%#010x\r\n", (uint32_t)stack5+256*4);
	printf("START\r\n");
	

	
	//printStackPointers();
	
	OS_start();
}
