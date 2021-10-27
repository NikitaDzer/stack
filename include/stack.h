#ifndef STACK_H
#define STACK_H

#include <cstddef>
#include <cstdint>
#include "config.h"

#ifdef STK_UNPROTECT
#undef STK_CANARY_PROTECT
#undef STK_HASH_PROTECT
#endif

typedef size_t stk_bitmask_t;

#ifdef STK_CANARY_PROTECT
typedef size_t stk_canary_t;
#endif

#ifdef STK_HASH_PROTECT
typedef size_t stk_hash_t;
#endif

const char   STK_POISON          = 'u';
const size_t STK_MIN_CAPACITY    = 0;
const size_t STK_MAX_CAPACITY    = SIZE_MAX >> 1;

#ifdef STK_CANARY_PROTECT
const stk_canary_t  STK_CANARY          = 7171717171717171; //! change
#endif

struct Stack
{
#ifdef STK_CANARY_PROTECT
   stk_canary_t   leftCanary;
#endif
   
   size_t         size;
   size_t         capacity;
   size_t         minCapacity;
   size_t         bytes;
   void          *storage;

#ifdef STK_HASH_PROTECT
   stk_hash_t      hash;
#endif

#ifdef STK_CANARY_PROTECT
   stk_canary_t   rightCanary;
#endif
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

stk_bitmask_t stack_init(Stack *const p_stack, const size_t minCapacity = STK_MIN_CAPACITY);

stk_bitmask_t stack_push(Stack *const p_stack, const stk_element_t element);

stk_bitmask_t stack_pop(Stack *const p_stack, stk_element_t *const p_output);

void stack_destroy(Stack *const p_stack);


#endif // STACK_H
