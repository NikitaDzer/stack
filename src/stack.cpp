#include <cstdlib>
#include "../include/stack.h"
#include "../include/protect.h"

typedef stk_element_t element_t;

#ifdef STK_PRODUCTION
// --------------------------------  export functions  --------------------------------
void stack_init(Stack *const p_stack, const size_t userMinCapacity)
{
   size_t minCapacity = 0;
   
   if (userMinCapacity >= 1)
   {
      if (userMinCapacity / 2 * 2 == userMinCapacity)
         minCapacity = userMinCapacity;
      else
         minCapacity = (userMinCapacity / 2 + 1) * 2;
   }
   
   p_stack->size        = 0;
   p_stack->minCapacity = minCapacity;
   p_stack->capacity    = minCapacity;
   p_stack->bytes       = 0;
   p_stack->storage     = malloc(sizeof(element_t) * minCapacity);
}

void stack_push(Stack *const p_stack, const element_t element)
{
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size + 1;
   
   if (newSize > prevCapacity)
   {
      size_t newCapacity = 2;
      
      if (prevCapacity)
         newCapacity = prevCapacity * 2;
   
      p_stack->capacity = newCapacity;
      p_stack->storage = realloc(p_stack->storage, sizeof(element_t) * newCapacity);
   }
   
   p_stack->size = newSize;
   
   *((element_t *)p_stack->storage + newSize - 1) = element;
}

void stack_pop(Stack *const p_stack, element_t *const p_output)
{
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size - 1;
   size_t       newCapacity  = minCapacity;
   
   *p_output = *((element_t *)p_stack->storage + newSize);
   
   if (newSize <= minCapacity)
      newCapacity = minCapacity;
   else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
      newCapacity = prevCapacity / 2; //! opt
   else
      newCapacity = prevCapacity;
   
   if (prevCapacity != newCapacity)
   {
      p_stack->capacity = newCapacity;
      p_stack->storage  = realloc(p_stack->storage, sizeof(element_t) * newCapacity);
   }
   
   p_stack->size = newSize;
}

void stack_destroy(Stack *const p_stack)
{
   free(p_stack->storage);
   
   p_stack->size = 0;
   p_stack->capacity = 0;
   p_stack->minCapacity = 0;
   p_stack->bytes = 0;
   p_stack->storage = nullptr;
}
// --------------------------------  /export functions  --------------------------------
#else
// --------------------------------  includes  -------------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include "../include/protect.h"
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  -------------------------------------------------------------------------
typedef stk_bitmask_t bitmask_t;
typedef int_fast8_t  error_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
#endif
// -------------------------------- /typedefs  -------------------------------------------------------------------------


// --------------------------------  static functions  -----------------------------------------------------------------
static inline void* find_lastElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef    STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t) + sizeof(element_t) * (size - 1));
#else
   return (void *)((char *)storage + sizeof(element_t) * (size - 1));
#endif // STK_CANARY_PROTECT
}

static size_t calc_storage_bytes(const size_t capacity)
{
   if (capacity == 0)
      return 0;

#ifdef    STK_CANARY_PROTECT
   size_t       bytes           = 0;
   const size_t canary_bytes    = sizeof(canary_t);
   const size_t container_bytes = sizeof(element_t) * capacity;
   
   bytes = container_bytes / canary_bytes * canary_bytes;
   
   if (bytes != container_bytes)
      bytes += canary_bytes;
   
   bytes += canary_bytes * 2;
   
   return bytes;
#else
   return sizeof(element_t) * capacity;
#endif // STK_CANARY_PROTECT
}

static size_t calc_min_capacity(const size_t userMinCapacity)
{
   if (userMinCapacity == 0)
      return 0;
   
   size_t minCapacity = 0;
   
   if (userMinCapacity / 2 * 2 == userMinCapacity)
      minCapacity = userMinCapacity;
   else
      minCapacity = (userMinCapacity / 2 + 1) * 2;
   
   return minCapacity;
}

static error_t update_storage(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   size_t  size     = p_stack->size;
   size_t  capacity = p_stack->capacity;
   size_t  bytes    = 0;
   void   *storage  = nullptr;
   
   if (capacity == 0)
      free(p_stack->storage);
   else
   {
      bytes   = calc_storage_bytes(capacity);
      storage = realloc(p_stack->storage, bytes);
      
      if (storage == nullptr)
      {
         *p_bitmask |= STK_BITMASK_SYSTEM_ERROR;
         return -1;
      }
   
      void       *p_storage_poison      = nullptr;
      
      #ifdef    STK_CANARY_PROTECT
      void *const p_storage_leftCanary  = find_storage_leftCanary(storage);
      void *const p_storage_rightCanary = find_storage_rightCanary(storage, bytes);
      
      *(canary_t *)p_storage_leftCanary  = STK_CANARY;
      *(canary_t *)p_storage_rightCanary = STK_CANARY;
      
      if (size == 0)
         p_storage_poison = (canary_t *)p_storage_leftCanary + 1;
      else
         p_storage_poison = (element_t *)find_lastElement(storage, size) + 1;

      memset(p_storage_poison, STK_POISON, (char *)p_storage_rightCanary - (char *)p_storage_poison);
      #else  // !STK_CANARY_PROTECT
      if (size == 0)
         p_storage_poison = (element_t *)storage;
      else
         p_storage_poison = (element_t *)find_lastElement(storage, size) + 1;
      
      memset(p_storage_poison, STK_POISON, bytes - ((char *)p_storage_poison - (char *)storage));
      #endif // !STK_CANARY_PROTECT
   }
   
   #ifdef STK_CANARY_PROTECT
   p_stack->bytes   = bytes;
   #endif
   
   p_stack->storage = storage;
   
   return 0;
}

/*
static int stack_resize(Stack *const p_stack, const char operation, bitmask_t *const p_bitmask)
{
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   if (operation == '+')
   {
      if (prevSize == STK_MAX_CAPACITY_)
      {
         *p_bitmask |= StackDetails::STACK_FULL;
         return 1;
      }
      
      newSize = prevSize + 1;

      if (prevCapacity == 0)
         newCapacity = 2;
      else if (newSize <= prevCapacity)
         newCapacity = prevCapacity;
      else
         newCapacity = prevCapacity * 2;
   }
   else if (operation == '-')
   {
      if (prevSize == 0)
      {
         *p_bitmask |= StackDetails::STACK_EMPTY;
         return 1;
      }
      
      newSize = prevSize - 1;

      if (newSize <= minCapacity)
         newCapacity = minCapacity;
      else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
         newCapacity = prevCapacity / 2; //! opt
      else
         newCapacity = prevCapacity;
   }
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
#ifdef STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return update_storage(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif
}
*/

static error_t increase_storage(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size + 1;
   size_t       newCapacity  = 0;
   
   if (prevCapacity == 0)
      newCapacity = 2;
   else if (newSize <= prevCapacity)
      newCapacity = prevCapacity;
   else
      newCapacity = prevCapacity * 2;
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
   return update_storage(p_stack, p_bitmask);
}

static error_t decrease_storage(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   const size_t minCapacity  = p_stack->minCapacity; //! opt
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size - 1;
   size_t       newCapacity  = 0;
   
   if (newSize <= minCapacity)
      newCapacity = minCapacity;
   else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
      newCapacity = prevCapacity / 2; //! opt
   else
      newCapacity = prevCapacity;
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
   return update_storage(p_stack, p_bitmask);
}

bitmask_t stack_init_(Stack *const p_stack, const size_t userMinCapacity)
{
   bitmask_t bitmask = 0;
   
   #ifndef   STK_UNPROTECT
   if (checkpoint_init(p_stack, userMinCapacity, &bitmask))
      return bitmask;
   #endif // STK_UNPROTECT
   
   const size_t minCapacity = calc_min_capacity(userMinCapacity);
   
   p_stack->size        = 0;
   p_stack->capacity    = minCapacity;
   p_stack->minCapacity = minCapacity;
   p_stack->storage     = nullptr;
   
   #ifdef    STK_CANARY_PROTECT
   p_stack->bytes       = 0;
   p_stack->leftCanary  = STK_CANARY;
   p_stack->rightCanary = STK_CANARY;
   #endif // STK_CANARY_PROTECT
   
   #ifdef    STK_HASH_PROTECT
   p_stack->hash = 0;
   #endif // STK_HASH_PROTECT
   
   update_storage(p_stack, &bitmask);
   
   #ifndef   STK_UNPROTECT
   protocol_init(p_stack, &bitmask);
   #endif // !STK_UNPROTECT

   return bitmask;
}

bitmask_t stack_push_(Stack *const p_stack, const element_t element)
{
   bitmask_t bitmask = 0;
   
   
   #ifndef   STK_UNPROTECT
   if (checkpoint_push(p_stack, &bitmask)) {
      return bitmask;
   }
   #endif // !STK_UNPROTECT
   
   if (increase_storage(p_stack, &bitmask) == 0) {
      *(element_t *)find_lastElement(p_stack->storage, p_stack->size) = element;
   }

   #ifndef   STK_UNPROTECT
   protocol_push(p_stack, &bitmask);
   #endif // !STK_UNPROTECT
   
   
   return bitmask;
}

bitmask_t stack_pop_(Stack *const p_stack, element_t *const p_output)
{
  bitmask_t bitmask = 0;
  
   #ifndef   STK_UNPROTECT
   if (checkpoint_pop(p_stack, p_output, &bitmask))
     return bitmask;
   #endif // !STK_UNPROTECT
  
   *p_output = *(element_t *)find_lastElement(p_stack->storage, p_stack->size);
   
   decrease_storage(p_stack, &bitmask);

   #ifndef   STK_UNPROTECT
   protocol_pop(p_stack, &bitmask);
   #endif // !STK_UNPROTECT

  return bitmask;
}

void stack_destroy_(Stack *const p_stack)
{
   if (p_stack != nullptr)
   {
      free(p_stack->storage);
      
      p_stack->size          = 0;
      p_stack->capacity      = 0;
      p_stack->minCapacity   = 0;
      p_stack->storage       = nullptr;
      
      #ifdef    STK_CANARY_PROTECT
      p_stack->bytes         = 0;
      p_stack->leftCanary    = 0;
      p_stack->rightCanary   = 0;
      #endif // STK_CANARY_PROTECT
      
      #ifdef    STK_HASH_PROTECT
      p_stack->hash          = 0;
      #endif // STK_HASH_PROTECT
   
      #ifndef   STK_UNPROTECT
      protocol_destroy(p_stack);
      #endif // !STK_UNPROTECT
   }
}
// -------------------------------- /static functions  -----------------------------------------------------------------


// --------------------------------  export functions  -----------------------------------------------------------------
#ifndef    STK_DEBUG
bitmask_t stack_init(Stack *const p_stack, const size_t userMinCapacity)
{
   return stack_init_(p_stack, userMinCapacity);
}

bitmask_t stack_push(Stack *const p_stack, const element_t element)
{
   return stack_push_(p_stack, element);
}

bitmask_t stack_pop(Stack *const p_stack, element_t *const p_output)
{
   return stack_pop_(p_stack, p_output);
}

void stack_destroy(Stack *const p_stack)
{
   stack_destroy_(p_stack);
}
#endif // STK_DEBUG
// -------------------------------- /export functions  -----------------------------------------------------------------
#endif
