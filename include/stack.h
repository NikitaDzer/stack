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
   stk_element_t *elements;
};

/// global constants ->

const stk_element_t STK_INIT_DATA         = 'u';
const size_t        STK_SECTION_CAPACITY  = 16;

enum StackStatementResult {
   STK_SUCCESS,
   STK_ERROR__NULLPTR,
   STK_ERROR__UNALLOCATED_MEMORY,
   STK_ERROR__REINITIALIZATION,
   STK_ERROR__UNINITIALIZED,
   STK_ERROR__ELEMENTS_NULLPTR
};

/// <- global constants


#endif // STACK_H
