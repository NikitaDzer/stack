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
   size_t         cock;
   size_t         size;
   size_t         capacity;
   size_t         minCapacity;
   stk_element_t *storage;
   size_t         storage_bytes;
   size_t         hen;
};

/// global constants ->

const stk_element_t STK_POISON          = 'u';
const size_t        STK_COCK_HEN        = 12121212121212121212; //! change
const size_t        STK_MIN_CAPACITY    = 0;
const size_t        STK_MAX_CAPACITY    = SIZE_MAX >> 1;

enum StackError {
   NOERR,
   NULLPTR,
   UNINITIALIZED,
   REINITIALIZATION,
   EMPTY,
   MAX_SIZE,
   NON_UPDATED,
   NON_ALLOCATED,
   BANNED,
   OUTPUT_NULLPTR,
};

enum StackTypes {
   STK_TYPE__INT,
   STK_TYPE__CHAR,
   STK_TYPE__DOUBLE
};

/// <- global constants


#endif // STACK_H
