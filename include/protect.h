//
// Created by User on 02.11.2021.
//

#ifndef STACK_PROTECT_H
#define STACK_PROTECT_H

#include <cstdio>
#include <cmath>
#include "config.h"
#include "stack.h"

#if  defined(STK_CANARY_PROTECT) || defined(STK_HASH_PROTECT)
#define STK_INSPECTOR
#endif
// --------------------------------  includes  -------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>

#ifdef    STK_HASH_PROTECT
#include "hash.h"
#endif // STK_HASH_PROTECT
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  --------------------------------------------------------------------------
typedef int_fast8_t   error_t;
typedef size_t        stk_bitmask_t;

#ifdef    STK_CANARY_PROTECT
typedef size_t stk_canary_t;
#endif // STK_CANARY_PROTECT
// -------------------------------- /typedefs  -------------------------------------------------------------------------


// --------------------------------  constants  ------------------------------------------------------------------------
#ifdef STK_CANARY_PROTECT
const stk_canary_t  STK_CANARY = 0xD1AB011CB13D;
#endif // STK_CANARY_PROTECT

enum StackStatus
{
   WORKER             = 0 << 0,
   BANNED             = 1 << 0,
   CANDIDATE          = 1 << 1,
};

#ifdef STK_ANTI_FOOL_PROTECT
enum StackAntiFool
{
   STACK_FULL                   = 1 << 2,
   STACK_EMPTY                  = 1 << 3,
   BAD_STACK_POINTER            = 1 << 4,
   BAD_OUTPUT_POINTER           = 1 << 5,
   BAD_MIN_CAPACITY             = 1 << 6,
   BAD_WORKER                   = 1 << 7,
   BAD_CANDIDATE                = 1 << 8,
};
#endif
// -------------------------------- /constants  ------------------------------------------------------------------------


// --------------------------------  structures  -----------------------------------------------------------------------

// -------------------------------- /structures  -----------------------------------------------------------------------


// --------------------------------  export functions  -----------------------------------------------------------------
#ifdef    STK_CANARY_PROTECT
inline void* find_storage_leftCanary(const void *const storage);

inline void* find_storage_rightCanary(const void *const storage, const size_t bytes);
#endif // STK_CANARY_PROTECT

error_t check_canaries(const Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t check_hash(const Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t calc_hash(const Stack *const p_stack, hash_t *const p_output);

error_t checkpoint_init(const Stack *const p_stack, const size_t userMinCapacity, stk_bitmask_t *const p_bitmask);

error_t checkpoint_push(const Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t checkpoint_pop(const Stack *const p_stack, const stk_element_t *const p_output, stk_bitmask_t *const p_bitmask);

error_t protocol_init(Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t protocol_push(Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t protocol_pop(Stack *const p_stack, stk_bitmask_t *const p_bitmask);

error_t protocol_destroy(const Stack *const p_stack);
// -------------------------------- /export functions  -----------------------------------------------------------------
#endif //STACK_PROTECT_H
