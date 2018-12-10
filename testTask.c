#include "sleep.h"
#include "circularBuffer.h"

void task6(void const *const args) {
	while (1) {
		test = 2;
		OS_sleep(1000);
	}
}