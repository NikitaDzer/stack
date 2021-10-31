#ifndef STACK_DUMP_H
#define STACK_DUMP_H

#include "stack.h"

#ifdef    STK_DEBUG

enum StackLogCallers
{
   CALLER_NOONE,
   CALLER_STACK_INIT,
   CALLER_STACK_PUSH,
   CALLER_STACK_POP,
   CALLER_STACK_DESTROY
};

void stack_dump(const Stack *const p_stack);

stk_bitmask_t stack_log(const Stack *const p_stack, const StackLogCallers caller,
                        const char *const filename, const long line, const stk_bitmask_t bitmask, ...);
#else

#define stack_dump(p_stack)

#endif // STK_DEBUG

#endif // STACK_DUMP_H
