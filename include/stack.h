#ifndef STACK_H
#define STACK_H

#include <cstddef>
#define STK_SPECIFIER "d"

/*!
 * @brief Typedef for stack element's data type
 */
typedef int stk_element_t;


/*!
 * @brief Stack
 */
struct Stack {
   size_t size;
   size_t capacity;
   size_t minCapacity;
   stk_element_t *elements;
};

/// global constants ->

const stk_element_t STK_POISON           = 'u';
const size_t        STK_MIN_CAPACITY     = 0;
const size_t        STK_INIT_CAPACITY    = STK_MIN_CAPACITY;

enum StackStatementResult {
   STK_SUCCESS,
   STK_ERROR__STACK_NULLPTR,
   STK_ERROR__STACK_UNINITIALIZED,
   STK_ERROR__STACK_REINITIALIZATION,
   STK_ERROR__STACK_EMPTY,
   STK_ERROR__STACK_ELEMENTS_NULLPTR,
   STK_ERROR__UNALLOCATED_MEMORY,
   STK_ERROR__OUTPUT_NULLPTR,
   STK_ERROR__STACK_MISUSE,
   STK_ERROR__STACK_SIZE_MAX
};

enum StackTypes {
   STK_TYPE__INT,
   STK_TYPE__CHAR,
   STK_TYPE__DOUBLE
};

/// <- global constants


#endif // STACK_H
