#ifndef __interrupt_h__
#define __interrupt_h__
#include "os.h"
#include "os_internal.h"

void __svc(SVC_INTERRUPT_TEST) interrupt_test(void* mutex);

#endif /* __interrupt_h__ */