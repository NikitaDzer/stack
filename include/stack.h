#ifndef STACK_H
#define STACK_H

#include "config.h"
#include <cstdio>

#ifdef STK_PRODUCTION

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

#include <cstddef>
#include <cstdint>
#include <valarray>

#ifdef STK_HASH_PROTECT
#include "hash.h"
#endif

#ifdef STK_UNPROTECT
#undef STK_CANARY_PROTECT
#undef STK_HASH_PROTECT
#endif // STK_UNPROTECT

typedef size_t stk_bitmask_t;

const char   STK_POISON        = 0x66;
const size_t STK_MIN_CAPACITY_ = (STK_MIN_CAPACITY & (STK_MIN_CAPACITY - 1)) == 0
                                 ? STK_MIN_CAPACITY
                                 : pow(2, ceil(log(STK_MIN_CAPACITY) / log(2)));
const size_t STK_MAX_CAPACITY_ = STK_MIN_CAPACITY > STK_MAX_CAPACITY
                                 ? STK_MIN_CAPACITY_
                                 : STK_MAX_CAPACITY > (SIZE_MAX >> 1)
                                    ? (SIZE_MAX >> 1) + 1
                                    : pow(2, ceil(log(STK_MAX_CAPACITY) / log(2)));

#ifdef STK_CANARY_PROTECT
typedef size_t stk_canary_t;

const stk_canary_t  STK_CANARY = 0xD1AB011CB13D;
#endif // STK_CANARY_PROTECT

#ifndef SPECIFIER
   #ifdef STK_ELEMENT_SPECIFIER
      #define SPECIFIER STK_ELEMENT_SPECIFIER
   #endif
#endif

struct Stack
{
#ifdef STK_CANARY_PROTECT
   stk_canary_t   leftCanary;
#endif // STK_CANARY_PROTECT
   
   size_t         size;
   size_t         capacity;
   size_t         minCapacity;
   size_t         bytes;
   void          *storage;

#ifdef STK_HASH_PROTECT
   hash_t         hash;
#endif // STK_HASH_PROTECT

#ifdef STK_CANARY_PROTECT
   stk_canary_t   rightCanary;
#endif // STK_CANARY_PROTECT
};

enum StackDetails
{
   HEALTHY                        = 0 << 0,
   BANNED                         = 1 << 0,
   EMPTY                          = 1 << 1,
   FULL                           = 1 << 2,
   LEFT_CANARY_ATTACKED           = 1 << 3,
   RIGHT_CANARY_ATTACKED          = 1 << 4,
   STORAGE_LEFT_CANARY_ATTACKED   = 1 << 5,
   STORAGE_RIGHT_CANARY_ATTACKED  = 1 << 6,
};

enum StackStatementDetails
{
   SUCCESS                            = 0 << 16,
   ERROR                              = 1 << 16,
   STACK_NULLPTR                      = 1 << 17,
   OUTPUT_NULLPTR                     = 1 << 18,
   REINITIALIZATION                   = 1 << 19,
   WRONG_MIN_CAPACITY                 = 1 << 20,
   STORAGE_NOT_UPDATED                = 1 << 21,
   STATEMENT_WITH_BANNED_STACK        = 1 << 22,
   UNINITIALIZED_STACK                = 1 << 23,
   MEMORY_NOT_ALLOCATED               = 1 << 24,
   HASH_NOT_VERIFIED                  = 1 << 25,
};

void stack_dump(const Stack *const p_stack, const char *const file, const int line );

stk_bitmask_t stack_init(Stack *const p_stack, const size_t minCapacity = STK_MIN_CAPACITY_);

stk_bitmask_t stack_push(Stack *const p_stack, const stk_element_t element);

stk_bitmask_t stack_pop(Stack *const p_stack, stk_element_t *const p_output);

void stack_destroy(Stack *const p_stack);

#endif // STK_PRODUCTION
#endif // STACK_H
