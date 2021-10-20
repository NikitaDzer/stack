#ifndef STACK_FUNCTIONS_H
#define STACK_FUNCTIONS_H


#include "stack.h"

/*!
 * @brief Prints information about putted stack
 * @param stack Pointer of stack to be print
 */
void stack_dump( const Stack *const p_stack, const char *const file, const int line );


/*!
 * @brief Constructor for new stack
 * @param p_stack  Stack pointer
 * @param size     Stack size
 * @param initData Stack initial data
 * @return
 */
StackStatementResult stack_init(Stack *const p_stack, const size_t minCapacity = STK_MIN_CAPACITY);

StackStatementResult stack_push(Stack *const p_stack, const stk_element_t element);

StackStatementResult stack_pop(Stack *const p_stack, stk_element_t *const p_output);

StackStatementResult stack_kill(Stack *const p_stack);

#endif // STACK_FUNCTIONS_H
