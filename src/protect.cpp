



#ifndef STK_PRODUCTION
// --------------------------------  includes  -------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "../include/stack.h"
#include "../include/protect.h"
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  -------------------------------------------------------------------------
typedef stk_bitmask_t bitmask_t;
typedef stk_element_t element_t;
typedef int_fast8_t  error_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
#endif
// --------------------------------  structures  -----------------------------------------------------------------------
struct StackTracker
{
   const Stack *p_stack;
   StackStatus  stack_status;
};
// -------------------------------- /structures  -----------------------------------------------------------------------

#ifdef STK_ANTI_FOOL_PROTECT
// --------------------------------  static variables  -----------------------------------------------------------------
static StackTracker  staticTrackers[STK_STATIC_TRACKERS_NUMBER] = {};
static StackTracker *heapTrackers                               = nullptr;
static StackTracker *p_prevTracker                              = nullptr;
static size_t        trackers_counter                           = 0;
// -------------------------------- /static variables  -----------------------------------------------------------------
#endif

// --------------------------------  static functions  -----------------------------------------------------------------
#ifdef    STK_CANARY_PROTECT
inline void* find_storage_leftCanary(const void *const storage)
{
   return (void *)storage;
}

inline void* find_storage_rightCanary(const void *const storage, const size_t bytes)
{
   if (storage == nullptr)
      return nullptr;
   
   return (void *)((char *)storage + bytes - sizeof(canary_t));
}
#endif // STK_CANARY_PROTECT

#ifdef STK_ANTI_FOOL_PROTECT
static StackTracker* find_tracker(const Stack *const p_stack)
{
   if (p_prevTracker != nullptr && p_prevTracker->p_stack == p_stack)
      return p_prevTracker;
   
   if (trackers_counter > STK_STATIC_TRACKERS_NUMBER)
   {
      size_t index = 0;
      
      for (index = 0; index < STK_STATIC_TRACKERS_NUMBER; index++)
         if (staticTrackers[index].p_stack == p_stack)
            return (StackTracker *)staticTrackers + index;
      
      for (index = index; index < trackers_counter; index++)
         if (heapTrackers[index - STK_STATIC_TRACKERS_NUMBER].p_stack == p_stack)
            return (StackTracker *)heapTrackers + index - STK_STATIC_TRACKERS_NUMBER;
   }
   else
   {
      for (size_t index = 0; index < trackers_counter; index++)
         if (staticTrackers[index].p_stack == p_stack)
            return (StackTracker *)staticTrackers + index;
   }
   
   return nullptr;
}

static StackTracker* create_tracker(const Stack *const p_stack)
{
   StackTracker *p_tracker = nullptr;
   
   trackers_counter += 1;
   
   if (trackers_counter > STK_STATIC_TRACKERS_NUMBER)
   {
      heapTrackers = (StackTracker *)realloc(heapTrackers, sizeof(StackTracker) * trackers_counter);
      p_tracker    = (StackTracker *)heapTrackers + trackers_counter - STK_STATIC_TRACKERS_NUMBER - 1;
   }
   else
   {
      p_tracker = (StackTracker *)staticTrackers + trackers_counter - 1;
   }
   
   p_tracker->p_stack = p_stack;
   p_tracker->stack_status   = StackStatus::CANDIDATE;
   
   return p_tracker;
}

static StackTracker* get_tracker(const Stack *const p_stack)
{
   StackTracker* p_tracker = find_tracker(p_stack);
   
   if (p_tracker == nullptr)
      p_tracker = create_tracker(p_stack);
   
   p_prevTracker = p_tracker;
   
   return p_tracker;
}

static void update_tracker(const Stack *const p_stack, const StackStatus status)
{
   if (p_prevTracker->p_stack == p_stack)
      p_prevTracker->stack_status = status;
   else
   {
      StackTracker *const p_tracker = find_tracker(p_stack);
      
      p_tracker->stack_status = status;
      p_prevTracker = p_tracker;
   }
}
#endif

#ifdef    STK_HASH_PROTECT
error_t calc_hash(const Stack *const p_stack, hash_t *const p_output)
{
#ifdef    STK_CANARY_PROTECT
   const size_t bytes = p_stack->bytes;
#else
   const size_t bytes = sizeof(element_t) * p_stack->capacity;
#endif // STK_CANARY_PROTECT
   
   void *const buffer = malloc(sizeof(Stack) + bytes);
   
   if (buffer == nullptr) {
      return -1;
   }
   
   memcpy(buffer, p_stack, sizeof(Stack));
   memset((char *)buffer + ((char *)&p_stack->hash - (char *)p_stack), 0x00, sizeof(hash_t));
   
   if (p_stack->storage != nullptr)
      memcpy((char *)buffer + sizeof(Stack), p_stack->storage, bytes);
   
   *p_output = qhash(buffer, sizeof(Stack) + bytes);
   
   free(buffer);
   
   return 0;
}
#endif // STK_HASH_PROTECT

#ifdef   STK_CANARY_PROTECT
error_t check_canaries(const Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (   p_stack->leftCanary != STK_CANARY
          || p_stack->rightCanary != STK_CANARY
          || p_stack->storage != nullptr
             && (   *(canary_t *)find_storage_leftCanary(p_stack->storage) != STK_CANARY
                    || *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) != STK_CANARY))
   {
      *p_bitmask |= StackStatus::BANNED;
      return 1;
   }
   
   return 0;
}
#endif

#ifdef STK_HASH_PROTECT
error_t check_hash(const Stack *const p_stack, bitmask_t *const p_bitmask)
{
   hash_t checkHash = 0;
   
   printf("o");
   if (calc_hash(p_stack, &checkHash))
   {
      *p_bitmask |= STK_BITMASK_SYSTEM_ERROR;
      *p_bitmask |= StackStatus::BANNED;
      return -1;
   }
   
   if (p_stack->hash != checkHash)
   {
      *p_bitmask |= StackStatus::BANNED;
      return 1;
   }
   
   return 0;
}
#endif

#ifdef STK_ANTI_FOOL_PROTECT
static error_t foolproof_init(const Stack *const p_stack, const size_t userMinCapacity, const StackStatus status, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;
   
   if (p_stack == nullptr)
   {
      localBitmask |= StackAntiFool::BAD_STACK_POINTER;
   }
   
   if (userMinCapacity > STK_MAX_CAPACITY_)
   {
      localBitmask |= StackAntiFool::BAD_MIN_CAPACITY;
   }
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      return 1;
   }
   
   if (status != StackStatus::CANDIDATE)
   {
      *p_bitmask |= status;
      *p_bitmask |= StackAntiFool::BAD_CANDIDATE;
      return 1;
   }
   
   return 0;
}

static error_t foolproof_push(const Stack *const p_stack, const StackStatus status, bitmask_t *const p_bitmask)
{
   if (p_stack == nullptr)
   {
      *p_bitmask |= StackAntiFool::BAD_STACK_POINTER;
      return 1;
   }
   
   if (status != StackStatus::WORKER)
   {
      *p_bitmask |= status;
      *p_bitmask |= StackAntiFool::BAD_WORKER;
      return 1;
   }
   
   if (p_stack->size == STK_MAX_CAPACITY_)
   {
      *p_bitmask |= StackAntiFool::STACK_FULL;
      return 1;
   }
   
   return 0;
}

static error_t foolproof_pop(const Stack *const p_stack, const element_t *p_output, const StackStatus status, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;
   
   if (p_stack == nullptr)
   {
      localBitmask |= StackAntiFool::BAD_STACK_POINTER;
   }
   
   if (p_output == nullptr)
   {
      localBitmask |= StackAntiFool::BAD_OUTPUT_POINTER;
   }
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      return 1;
   }
   
   if (status != StackStatus::WORKER)
   {
      *p_bitmask |= status;
      *p_bitmask |= StackAntiFool::BAD_WORKER;
      return 1;
   }
   
   if (p_stack->size == 0)
   {
      *p_bitmask |= StackAntiFool::STACK_EMPTY;
      return 1;
   }
   
   return 0;
}
#endif

#ifdef STK_INSPECTOR
static error_t inspect_stack(const Stack *const p_stack, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;

   #ifdef STK_CANARY_PROTECT
   check_canaries(p_stack, &localBitmask);
   #endif

   #ifdef STK_HASH_PROTECT
   check_hash(p_stack, &localBitmask);
   #endif
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      
      if (localBitmask & STK_BITMASK_SYSTEM_ERROR)
         return -1;
      
      return 1;
   }
   
   return 0;
}
#endif // STK_UNPROTECT

error_t checkpoint_init(const Stack *const p_stack, const size_t userMinCapacity, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;

   #ifdef STK_ANTI_FOOL_PROTECT
   StackTracker *p_tracker = get_tracker(p_stack);
   
   if (foolproof_init(p_stack, userMinCapacity, p_tracker->stack_status, &localBitmask))
   {
      #ifdef  STK_INSPECTOR
      if (p_tracker->stack_status == StackStatus::WORKER)
         if (inspect_stack(p_stack, &localBitmask))
            update_tracker(p_stack, StackStatus::BANNED);
      #endif
   }
   #endif
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      
      if (localBitmask & STK_BITMASK_SYSTEM_ERROR)
         return -1;
      
      return 1;
   }
   
   return 0;
}

error_t checkpoint_push(const Stack *const p_stack, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;
   
   #ifdef    STK_ANTI_FOOL_PROTECT
   StackTracker *p_tracker = get_tracker(p_stack);
   
   foolproof_push(p_stack, p_tracker->stack_status, &localBitmask);
   
   #ifdef    STK_INSPECTOR
   if (p_tracker->stack_status == StackStatus::WORKER)
   {
      if (inspect_stack(p_stack, &localBitmask))
      {
         localBitmask &= ~StackAntiFool::STACK_FULL;
         update_tracker(p_stack, StackStatus::BANNED);
      }
      
   }
   #endif // STK_INSPECTOR
   #else // !STK_ANTI_FOOL_PROTECT
   inspect_stack(p_stack, &localBitmask);
   #endif // !STK_ANTI_FOOL_PROTECT
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      
      if (localBitmask & STK_BITMASK_SYSTEM_ERROR)
         return -1;
      
      return 1;
   }
   
   return 0;
}

error_t checkpoint_pop(const Stack *const p_stack, const element_t *const p_output, bitmask_t *const p_bitmask)
{
   bitmask_t localBitmask = 0;

   #ifdef    STK_ANTI_FOOL_PROTECT
   StackTracker *p_tracker = get_tracker(p_stack);
   
   foolproof_pop(p_stack, p_output, p_tracker->stack_status, &localBitmask);

   #ifdef    STK_INSPECTOR
   if (p_tracker->stack_status == StackStatus::WORKER)
   {
      if (inspect_stack(p_stack, &localBitmask))
      {
         localBitmask &= ~StackAntiFool::STACK_EMPTY;
         update_tracker(p_stack, StackStatus::BANNED);
      }
   }
   #endif // STK_INSPECTOR
   #else // !STK_ANTI_FOOL_PROTECT
   inspect_stack(p_stack, &localBitmask);
   #endif // !STK_ANTI_FOOL_PROTECT
   
   if (localBitmask != 0)
   {
      *p_bitmask |= localBitmask;
      
      if (localBitmask & STK_BITMASK_SYSTEM_ERROR)
         return -1;
      
      return 1;
   }
   
   return 0;
}

error_t protocol_init(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (*p_bitmask == 0)
   {
      #ifdef STK_HASH_PROTECT
      if (calc_hash(p_stack, &p_stack->hash))
      {
         *p_bitmask |= StackStatus::CANDIDATE;
         *p_bitmask |= STK_BITMASK_SYSTEM_ERROR;
         return -1;
      }
      #endif // STK_HASH_PROTECT

      #ifdef STK_ANTI_FOOL_PROTECT
      update_tracker(p_stack, StackStatus::WORKER);
      #endif
   }
   else
   {
      *p_bitmask |= StackStatus::CANDIDATE;
      return 1;
   }
   
   return 0;
}

error_t protocol_push(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (*p_bitmask != 0)
   {
      #ifdef    STK_ANTI_FOOL_PROTECT
      update_tracker(p_stack, StackStatus::BANNED);
      #endif // STK_ANTI_FOOL_PROTECT
   
      *p_bitmask |= StackStatus::BANNED;
      return -1;
   }

   #ifdef    STK_HASH_PROTECT
   if (calc_hash(p_stack, &p_stack->hash))
   {
      #ifdef    STK_ANTI_FOOL_PROTECT
      update_tracker(p_stack, StackStatus::BANNED);
      #endif // STK_ANTI_FOOL_PROTECT
      
      *p_bitmask |= StackStatus::BANNED;
      *p_bitmask |= STK_BITMASK_SYSTEM_ERROR;
      
      return -1;
   }
   #endif // STK_HASH_PROTECT
   
   return 0;
}

error_t protocol_pop(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (*p_bitmask != 0)
   {
      #ifdef    STK_ANTI_FOOL_PROTECT
       update_tracker(p_stack, StackStatus::BANNED);
      #endif // STK_ANTI_FOOL_PROTECT
      
      *p_bitmask |= StackStatus::BANNED;
      return -1;
   }

   #ifdef    STK_HASH_PROTECT
   if (calc_hash(p_stack, &p_stack->hash))
   {
      #ifdef    STK_ANTI_FOOL_PROTECT
      update_tracker(p_stack, StackStatus::BANNED);
      #endif // STK_ANTI_FOOL_PROTECT
      
      *p_bitmask |= StackStatus::BANNED;
      *p_bitmask |= STK_BITMASK_SYSTEM_ERROR;
      
      return -1;
   }
   #endif // STK_HASH_PROTECT
   
   return 0;
}

error_t protocol_destroy(const Stack *const p_stack)
{
   #ifdef STK_ANTI_FOOL_PROTECT
   update_tracker(p_stack, StackStatus::CANDIDATE);
   #endif
   
   return 0;
}

// -------------------------------- /static functions  -----------------------------------------------------------------



// --------------------------------  export functions  -----------------------------------------------------------------
#ifdef    STK_DEBUG

#else

#endif // STK_DEBUG
// -------------------------------- /export functions  -----------------------------------------------------------------
#endif
