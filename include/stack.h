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
   size_t cock;
   size_t size;
   size_t capacity;
   size_t minCapacity;
   stk_element_t *elements;
   size_t hen;
};

/// global constants ->

const stk_element_t STK_POISON          = 'u';
const size_t        STK_MIN_CAPACITY    = 0;
const size_t        STK_MAX_CAPACITY    = SIZE_MAX >> 1;
const size_t        STK_COCK_HEN_SHIELD = 2264114400;

enum StackStatementResult {
   SUCCESS,
   STACK_NULLPTR,
   STACK_UNINITIALIZED,
   STACK_REINITIALIZATION,
   STACK_EMPTY,
   STACK_ELEMENTS_NULLPTR,
   UNALLOCATED_MEMORY,
   OUTPUT_NULLPTR,
   STACK_MISUSE,
   STACK_EXCEEDED_MAX_CAPACITY,
   STACK_NON_UPDATED_ELEMENTS
};

enum StackTypes {
   STK_TYPE__INT,
   STK_TYPE__CHAR,
   STK_TYPE__DOUBLE
};

/// <- global constants


#endif // STACK_H
