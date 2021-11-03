#ifndef STACK_H
#define STACK_H

#include <cstdio>
#include <cmath>
#include "config.h"

#ifdef    STK_ELEMENT_SPECIFIER
#define SPECIFIER STK_ELEMENT_SPECIFIER
#endif // STK_ELEMENT_SPECIFIER


#ifdef    STK_PRODUCTION
#undef STK_UNPROTECT
#undef STK_CANARY_PROTECT
#undef STK_HASH_PROTECT

struct Stack
{
   size_t size;
   size_t capacity;
   size_t minCapacity;
   size_t bytes;
   void  *storage;
};

void stack_init(Stack *const p_stack, const size_t userMinCapacity = STK_MIN_CAPACITY_);

void stack_push(Stack *const p_stack, const stk_element_t element);

void stack_pop(Stack *const p_stack, stk_element_t *const p_output);

void stack_destroy(Stack *const p_stack);
#else
#ifdef    STK_UNPROTECT
#undef STK_ANTI_FOOL_PROTECT
#undef STK_CANARY_PROTECT
#undef STK_HASH_PROTECT
#endif // STK_UNPROTECT

typedef size_t stk_bitmask_t;
struct Stack;

// --------------------------------  includes  -------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>

#include "dump.h"

#ifndef   STK_UNPROTECT
#include "protect.h"
#endif // STK_UNPROTECT
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  --------------------------------------------------------------------------
typedef size_t stk_bitmask_t;
// -------------------------------- /typedefs  -------------------------------------------------------------------------


// --------------------------------  constants  ------------------------------------------------------------------------
const char          STK_POISON               = 0x66;
const stk_bitmask_t STK_BITMASK_SYSTEM_ERROR = 1ULL << (sizeof(stk_bitmask_t) * CHAR_BIT - 1);

const size_t STK_MIN_CAPACITY_ = (STK_MIN_CAPACITY & (STK_MIN_CAPACITY - 1)) == 0
                                 ? STK_MIN_CAPACITY
                                 : pow(2, ceil(log(STK_MIN_CAPACITY) / log(2)));

const size_t STK_MAX_CAPACITY_ = STK_MIN_CAPACITY > STK_MAX_CAPACITY
                                 ? STK_MIN_CAPACITY_
                                 : STK_MAX_CAPACITY > (SIZE_MAX >> 1)
                                   ? (SIZE_MAX >> 1) + 1
                                   : pow(2, ceil(log(STK_MAX_CAPACITY) / log(2)));
// -------------------------------- /constants  ------------------------------------------------------------------------


// --------------------------------  structures  -----------------------------------------------------------------------
struct Stack
{
#ifdef STK_CANARY_PROTECT
   stk_canary_t leftCanary;
#endif // STK_CANARY_PROTECT
   
   size_t       size;
   size_t       capacity;
   size_t       minCapacity;
   
#ifdef STK_CANARY_PROTECT
   size_t       bytes;
#endif
   
   void        *storage;

#ifdef STK_HASH_PROTECT
   hash_t       hash;
#endif // STK_HASH_PROTECT

#ifdef STK_CANARY_PROTECT
   stk_canary_t rightCanary;
#endif // STK_CANARY_PROTECT
};
// -------------------------------- /structures  -----------------------------------------------------------------------


// --------------------------------  export functions  -----------------------------------------------------------------
#ifdef STK_DEBUG
#define stack_init(p_stack, minCapacity)                                                         \
        process_init((p_stack), (minCapacity), ( __FILE__), (__LINE__))

#define stack_push(p_stack, element)                                                             \
        process_push((p_stack), (element), ( __FILE__), (__LINE__))

#define stack_pop(p_stack, p_output)                                                             \
        process_pop((p_stack), (p_output), ( __FILE__), (__LINE__))

#define stack_destroy(p_stack)                                                             \
        process_destroy((p_stack), ( __FILE__), (__LINE__))

stk_bitmask_t stack_init_(Stack *const p_stack, const size_t minCapacity = STK_MIN_CAPACITY_);

stk_bitmask_t stack_push_(Stack *const p_stack, const stk_element_t element);

stk_bitmask_t stack_pop_(Stack *const p_stack, stk_element_t *const p_output);

void stack_destroy_(Stack *const p_stack);
#else
stk_bitmask_t stack_init(Stack *const p_stack, const size_t minCapacity = STK_MIN_CAPACITY_);

stk_bitmask_t stack_push(Stack *const p_stack, const stk_element_t element);

stk_bitmask_t stack_pop(Stack *const p_stack, stk_element_t *const p_output);

void stack_destroy(Stack *const p_stack);
#endif
// -------------------------------- /export functions  -----------------------------------------------------------------
#endif // STK_PRODUCTION
#endif // STACK_H
