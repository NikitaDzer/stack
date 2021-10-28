#include <cstdlib>
#include "../include/stack.h"

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

#ifdef STK_HASH_PROTECT
#include "../include/hash.h"
#endif
// -------------------------------- /includes  -------------------------------------------------------------------------


// --------------------------------  typedefs  -------------------------------------------------------------------------
typedef stk_bitmask_t bitmask_t;
typedef uint_fast8_t  error_t;

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

#ifdef    STK_CANARY_PROTECT
static inline void* find_storage_leftCanary(const void *const storage)
{
   return (void *)storage;
}

static inline void* find_storage_rightCanary(const void *const storage, const size_t bytes)
{
   if (storage == nullptr)
      return nullptr;
   
   return (void *)((char *)storage + bytes - sizeof(canary_t));
}
#endif // STK_CANARY_PROTECT

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

static error_t calc_min_capacity(const size_t userMinCapacity, size_t *const p_minCapacity, bitmask_t *const p_bitmask)
{
   if (userMinCapacity > STK_MAX_CAPACITY_)
   {
      *p_bitmask |= StackStatementDetails::WRONG_MIN_CAPACITY;
      return 1;
   }
   
   if (userMinCapacity == 0)
      *p_minCapacity = 0;
   else
   {
      size_t minCapacity = 0;
      
      if (userMinCapacity / 2 * 2 == userMinCapacity)
         minCapacity = userMinCapacity;
      else
         minCapacity = (userMinCapacity / 2 + 1) * 2;
      
      *p_minCapacity = minCapacity;
   }
   
   return 0;
}

#ifdef    STK_HASH_PROTECT
static error_t calc_stack_hash(const Stack *const p_stack, hash_t *const p_output)
{
   #ifdef    STK_CANARY_PROTECT
   const size_t bytes = p_stack->bytes;
   #else
   const size_t bytes = sizeof(element_t) * p_stack->capacity;
   #endif // STK_CANARY_PROTECT
   
   void *const data = malloc(sizeof(Stack) + bytes);
   
   if (data == nullptr) {
      return 1;
   }
   
   memcpy(data,                         p_stack,          sizeof(Stack));
   memcpy((char *)data + sizeof(Stack), p_stack->storage, bytes);
   memset((char *)data + ((char *)&p_stack->hash - (char *)p_stack), 0x00, sizeof(hash_t));
   
   *p_output = qhash(data, sizeof(Stack) + bytes);
   
   free(data);
   
   return 0;
}
#endif // STK_HASH_PROTECT

#ifndef   STK_UNPROTECT
static bool is_stack_initialized(const Stack *const p_stack)
{
   if (p_stack->size != 0)
   {
      return true;
   }
   
   if (p_stack->capacity != 0)
   {
      return true;
   }
   
   if (p_stack->minCapacity != 0)
   {
      return true;
   }
   
   if (p_stack->storage != nullptr)
   {
      return true;
   }


#ifdef    STK_CANARY_PROTECT
   if (p_stack->bytes != 0)
   {
      return true;
   }
   
   if (p_stack->leftCanary != 0)
   {
      return true;
   }
   
   if (p_stack->rightCanary != 0)
   {
      return true;
   }
#endif // STK_CANARY_PROTECT


#ifdef    STK_HASH_PROTECT
   if (p_stack->hash != 0)
   {
      return true;
   }
#endif // STK_HASH_PROTECT
   
   return false;
}

static error_t stack_inspect(const Stack *const p_stack, bitmask_t *const p_bitmask)
{
#ifdef STK_CANARY_PROTECT
   if (p_stack->leftCanary != STK_CANARY)
   {
      *p_bitmask |= StackDetails::BANNED;
   }
   
   if (p_stack->rightCanary != STK_CANARY)
   {
      *p_bitmask |= StackDetails::BANNED;
   }
   
   if (p_stack->storage != nullptr)
   {
      if (*(canary_t *)find_storage_leftCanary(p_stack->storage) != STK_CANARY)
      {
         *p_bitmask |= StackDetails::STORAGE_LEFT_CANARY_ATTACKED;
         *p_bitmask |= StackDetails::BANNED;
      }
      
      if (*(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) != STK_CANARY)
      {
         *p_bitmask |= StackDetails::STORAGE_RIGHT_CANARY_ATTACKED;
         *p_bitmask |= StackDetails::BANNED;
      }
   }
#endif

#ifdef STK_HASH_PROTECT
   size_t verifyHash = 0;
   
   /// ?change expression
   if (calc_stack_hash(p_stack, &verifyHash))
   {
      *p_bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
   }
   
   if (p_stack->hash != verifyHash)
   {
      *p_bitmask |= StackStatementDetails::HASH_NOT_VERIFIED | StackDetails::BANNED;
   }
#endif
   
   if (*p_bitmask != 0)
      return 1;
   
   return 0;
}
#endif // STK_UNPROTECT

#ifdef    STK_CANARY_PROTECT
static error_t storage_update(void **const p_storage, size_t *const p_bytes,
                              const size_t capacity, const size_t size, bitmask_t *const p_bitmask)
{
   size_t  bytes   = 0;
   void   *storage = nullptr;
   
   if (capacity == 0)
      free(*p_storage);
   else
   {
      bytes   = calc_storage_bytes(capacity);
      storage = realloc(*p_storage, bytes);
      
      if (storage == nullptr)
      {
         *p_bitmask |= StackStatementDetails::STORAGE_NOT_UPDATED;
         return 1;
      }
   
      void       *p_storage_poison      = nullptr;
      void *const p_storage_leftCanary  = find_storage_leftCanary(storage);
      void *const p_storage_rightCanary = find_storage_rightCanary(storage, bytes);
      
      *(canary_t *)p_storage_leftCanary  = STK_CANARY;
      *(canary_t *)p_storage_rightCanary = STK_CANARY;
      
      if (size == 0)
         p_storage_poison = (canary_t *)p_storage_leftCanary + 1;
      else
         p_storage_poison = (element_t *)find_lastElement(storage, size) + 1;

      memset(p_storage_poison, STK_POISON, (char *)p_storage_rightCanary - (char *)p_storage_poison);
   }
   
   *p_storage = storage;
   *p_bytes   = bytes;
   
   return 0;
}
#else
static int storage_update(void **const p_storage, const size_t capacity, const size_t size, bitmask_t *const p_bitmask)
{
   void   *storage = nullptr;
   
   if (capacity == 0)
      free(*p_storage);
   else
   {
      const size_t bytes = calc_storage_bytes(capacity);
      
      storage = realloc(*p_storage, bytes);
      
      if (storage == nullptr)
      {
         *p_bitmask |= StackStatementDetails::STORAGE_NOT_UPDATED;
         return 1;
      }
      
      void *p_storage_poison  = nullptr;
      
      if (size == 0)
         p_storage_poison = (element_t *)storage;
      else
         p_storage_poison = (element_t *)find_lastElement(storage, size) + 1;
      
      memset(p_storage_poison, STK_POISON, bytes - ((char *)p_storage_poison - (char *)storage));
   }
   
   *p_storage = storage;
   
   return 0;
}
#endif // STK_CANARY_PROTECT

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
         *p_bitmask |= StackDetails::FULL;
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
         *p_bitmask |= StackDetails::EMPTY;
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
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif
}
*/

static error_t stack_size_increase(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (p_stack->size == STK_MAX_CAPACITY_)
   {
      *p_bitmask |= StackDetails::FULL;
      return 1;
   }
   
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

#ifdef    STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif // STK_CANARY_PROTECT
}

static error_t stack_size_decrease(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (p_stack->size == 0)
   {
      *p_bitmask |= StackDetails::EMPTY;
      return 1;
   }
   
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

#ifdef    STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif // STK_CANARY_PROTECT
}
// -------------------------------- /static functions  -----------------------------------------------------------------


// --------------------------------  export functions  -----------------------------------------------------------------
bitmask_t stack_init(Stack *const p_stack, const size_t userMinCapacity)
{
   bitmask_t bitmask = 0;
   
   if (p_stack == nullptr) {
      return bitmask | StackStatementDetails::STACK_NULLPTR;
   }

#ifndef   STK_UNPROTECT
   if (is_stack_initialized(p_stack)) {
      return bitmask | StackStatementDetails::REINITIALIZATION;
   }
#endif // STK_UNPROTECT

   size_t minCapacity = 0;

   if (calc_min_capacity(userMinCapacity, &minCapacity, &bitmask))
      return bitmask;

   p_stack->minCapacity = minCapacity;
   p_stack->capacity    = minCapacity;
   p_stack->size        = 0;
   p_stack->storage     = nullptr;

#ifdef    STK_CANARY_PROTECT
   p_stack->leftCanary  = STK_CANARY;
   p_stack->bytes       = 0;
   p_stack->rightCanary = STK_CANARY;
   
   storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size, &bitmask);
#else
   storage_update(&p_stack->storage, p_stack->capacity, p_stack->size, &bitmask);
#endif // STK_CANARY_PROTECT


#ifdef    STK_HASH_PROTECT
   p_stack->hash = 0;

   if (calc_stack_hash(p_stack, &p_stack->hash))
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
#endif // STK_HASH_PROTECT

   return bitmask;
}

bitmask_t stack_push(Stack *const p_stack, const element_t element)
{
   bitmask_t bitmask = 0;

   if (p_stack == nullptr)
      return bitmask | StackStatementDetails::STACK_NULLPTR;

#ifndef   STK_UNPROTECT
   if (stack_inspect(p_stack, &bitmask))
      return bitmask;
#endif // STK_UNPROTECT
   
   if (stack_size_increase(p_stack, &bitmask) == 0)
      *(element_t *)find_lastElement(p_stack->storage, p_stack->size) = element;

#ifdef    STK_HASH_PROTECT
   if (calc_stack_hash(p_stack, &p_stack->hash))
   {
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
      bitmask |= StackDetails::BANNED;
   }
#endif // STK_HASH_PROTECT

   return bitmask;
}

bitmask_t stack_pop(Stack *const p_stack, element_t *const p_output)
{
   bitmask_t bitmask = 0;
   
   if (p_stack == nullptr)
      bitmask |= StackStatementDetails::STACK_NULLPTR;
   
   if (p_output == nullptr)
      bitmask |= StackStatementDetails::OUTPUT_NULLPTR;
   
   if (bitmask != 0)
      return bitmask;
   
#ifndef   STK_UNPROTECT
   if (stack_inspect(p_stack, &bitmask))
      return bitmask;
#endif // STK_UNPROTECT
   
   *p_output = *(element_t *)find_lastElement(p_stack->storage, p_stack->size);
   
   stack_size_decrease(p_stack, &bitmask);

#ifdef    STK_HASH_PROTECT
   if (calc_stack_hash(p_stack, &p_stack->hash))
   {
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
      bitmask |= StackDetails::BANNED;
   }
#endif // STK_HASH_PROTECT

   return bitmask;
}

void stack_destroy(Stack *const p_stack)
{
   if (p_stack != nullptr)
   {
      free(p_stack->storage);
      
      p_stack->size          = 0;
      p_stack->capacity      = 0;
      p_stack->minCapacity   = 0;
      p_stack->storage       = nullptr;
      
#ifdef    STK_CANARY_PROTECT
      p_stack->leftCanary    = 0;
      p_stack->bytes         = 0;
      p_stack->rightCanary   = 0;
#endif // STK_CANARY_PROTECT

#ifdef    STK_HASH_PROTECT
      p_stack->hash          = 0;
#endif // STK_HASH_PROTECT
   }
}
// -------------------------------- /export functions  -----------------------------------------------------------------
#endif
