#ifndef STACK_H
#define STACK_H

#include <cstddef>
#define STK_SPECIFIER "d"
#define STK_SECTIONS_ON 1

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

const stk_element_t STK_INIT_DATA         = 'u';
const size_t        STK_SECTION_CAPACITY  = 16;
const size_t        STK_MIN_CAPACITY      = STK_SECTION_CAPACITY;

enum StackStatementResult {
   STK_SUCCESS,
   STK_ERROR__STACK_NULLPTR,
   STK_ERROR__STACK_UNINITIALIZED,
   STK_ERROR__STACK_REINITIALIZATION,
   STK_ERROR__STACK_EMPTY,
   STK_ERROR__STACK_ELEMENTS_NULLPTR,
   STK_ERROR__UNALLOCATED_MEMORY,
   STK_ERROR__OUTPUT_NULLPTR,
   STK_ERROR__STACK_MISUSE
};

enum StackTypes {
   STK_TYPE__INT,
   STK_TYPE__FLOAT,
   STK_TYPE__DOUBLE,
   STK_TYPE__CHAR
};

/// <- global constants


#endif // STACK_H
