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
   stk_element_t *storage;
   size_t storage_size_in_bytes;
   size_t hen;
};

/// global constants ->

const stk_element_t STK_POISON          = 'u';
const size_t        STK_COCK_HEN        = 12121212121212121212; //! change
const size_t        STK_MIN_CAPACITY    = 0;
const size_t        STK_MAX_CAPACITY    = SIZE_MAX >> 1;

enum StackStatementResult {
   NOERR,
   STACK_NULLPTR,
   STACK_UNINITIALIZED,
   STACK_REINITIALIZATION,
   STACK_EMPTY,
   STACK_ELEMENTS_NULLPTR,
   UNALLOCATED_MEMORY,
   OUTPUT_NULLPTR,
   STACK_MISUSE,
   STACK_EXCEEDED_MAX_CAPACITY,
   STACK_NON_UPDATED_ELEMENTS,
   STACK_BANNED
};

enum StackTypes {
   STK_TYPE__INT,
   STK_TYPE__CHAR,
   STK_TYPE__DOUBLE
};

/// <- global constants


#endif // STACK_H
