#ifndef STACK_H
#define STACK_H

#include <cstdio>
#include <cmath>
#include "config.h"

#ifdef    STK_ELEMENT_SPECIFIER
#define SPECIFIER STK_ELEMENT_SPECIFIER
#endif // STK_ELEMENT_SPECIFIER

const size_t STK_MIN_CAPACITY_ = (STK_MIN_CAPACITY & (STK_MIN_CAPACITY - 1)) == 0
                                 ? STK_MIN_CAPACITY
                                 : pow(2, ceil(log(STK_MIN_CAPACITY) / log(2)));
const size_t STK_MAX_CAPACITY_ = STK_MIN_CAPACITY > STK_MAX_CAPACITY
                                 ? STK_MIN_CAPACITY_
                                 : STK_MAX_CAPACITY > (SIZE_MAX >> 1)
                                   ? (SIZE_MAX >> 1) + 1
                                   : pow(2, ceil(log(STK_MAX_CAPACITY) / log(2)));

struct Stack;

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
#undef STK_CANARY_PROTECT
#undef STK_HASH_PROTECT
#endif // STK_UNPROTECT

// --------------------------------  includes  -------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>

typedef size_t stk_bitmask_t;

#include "dump.h"

#ifdef    STK_HASH_PROTECT
#include "hash.h"
#endif // STK_HASH_PROTECT
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  --------------------------------------------------------------------------
typedef size_t stk_bitmask_t;

#ifdef    STK_CANARY_PROTECT
typedef size_t stk_canary_t;
#endif // STK_CANARY_PROTECT
// -------------------------------- /typedefs  -------------------------------------------------------------------------


// --------------------------------  constants  ------------------------------------------------------------------------
const char         STK_POISON = 0x66;

#ifdef STK_CANARY_PROTECT
const stk_canary_t STK_CANARY = 0xD1AB011CB13D;
#endif // STK_CANARY_PROTECT

enum StackDetails
{
   HEALTHY                        = 0 << 0,
   EMPTY                          = 1 << 1,
   FULL                           = 1 << 2,
   LEFT_CANARY_ATTACKED           = 1 << 3,
   RIGHT_CANARY_ATTACKED          = 1 << 4,
   STORAGE_LEFT_CANARY_ATTACKED   = 1 << 5,
   STORAGE_RIGHT_CANARY_ATTACKED  = 1 << 6,
};

enum StackStatementDetails
{
   SUCCESS                        = 0 << 16,
   ERROR                          = 1 << 16,
   STACK_NULLPTR                  = 1 << 17,
   OUTPUT_NULLPTR                 = 1 << 18,
   CANDIDATE_NOT_CLEAR            = 1 << 19,
   WRONG_MIN_CAPACITY             = 1 << 20,
   STORAGE_NOT_UPDATED            = 1 << 21,
   STATEMENT_WITH_BANNED_STACK    = 1 << 22,
   UNINITIALIZED_STACK            = 1 << 23,
   MEMORY_NOT_ALLOCATED           = 1 << 24,
   HASH_NOT_VERIFIED              = 1 << 25,
   REINITIALIZATION               = 1 << 26,
   STATEMENT_WITH_CANDIDATE_STACK = 1 << 27,
};
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
        stack_log((p_stack), CALLER_STACK_INIT, __FILE__, __LINE__, stack_init_((p_stack), (minCapacity)), (minCapacity))

#define stack_push(p_stack, element)                                                             \
        stack_log((p_stack), CALLER_STACK_PUSH, __FILE__, __LINE__, stack_push_((p_stack), (element)), (element))
        
#define stack_pop(p_stack, p_output)                                                              \
        stack_log((p_stack), CALLER_STACK_POP, __FILE__, __LINE__, stack_pop_((p_stack), (p_output)), (p_output), (#p_output))
        
#define stack_destroy(p_stack)                                                                   \
        stack_destroy_((p_stack)),                                                               \
        stack_log((p_stack), CALLER_STACK_DESTROY, __FILE__, __LINE__, 0)

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
