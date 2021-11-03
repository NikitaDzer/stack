#ifndef STACK_DUMP_H
#define STACK_DUMP_H

#include "stack.h"

#ifdef    STK_DEBUG

stk_bitmask_t process_init(Stack *const p_stack, const size_t userMinCapacity,
                       const char *const filename, const long line);

stk_bitmask_t process_push(Stack *const p_stack, const stk_element_t element,
                           const char *const filename, const long line);

stk_bitmask_t process_pop(Stack *const p_stack, stk_element_t *const p_output,
                      const char *const filename, const long line);

void process_destroy(Stack *const p_stack,
                     const char *const filename, const long line);

void stack_debug(const Stack *const p_stack);

//void stack_dump(const Stack *const p_stack);
//
//stk_bitmask_t stack_log(const Stack *const p_stack, const StackLogCallers caller,
//                        const char *const filename, const long line, const stk_bitmask_t bitmask, ...);
#else

#define stack_dump(p_stack)

#endif // STK_DEBUG

#endif // STACK_DUMP_H
