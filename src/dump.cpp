

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../include/dump.h"

#ifndef STK_PRODUCTION
typedef stk_element_t element_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
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
#endif

static inline void* find_firstElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t));
#else
   return (void *)storage;
#endif
}

static inline void* find_lastElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t) + sizeof(element_t) * (size - 1));
#else
   return (void *)((char *)storage + sizeof(element_t) * (size - 1));
#endif
}

#ifdef STK_HASH_PROTECT
static int calc_stack_hash(const Stack *const p_stack, hash_t *const p_hash)
{
#ifdef STK_CANARY_PROTECT
   const size_t        bytes = p_stack->bytes;
#else
   const size_t        bytes = sizeof(element_t) * p_stack->capacity;
#endif
   void         *const data  = malloc(sizeof(Stack) + bytes);
   
   if (data == nullptr) {
      return 1;
   }
   
   memcpy(data,                         p_stack,          sizeof(Stack));
   memcpy((char *)data + sizeof(Stack), p_stack->storage, bytes);
   memset((char *)data + ((char *)&p_stack->hash - (char *)p_stack), 0x00, sizeof(hash_t));
   
   *p_hash = qhash(data, sizeof(Stack) + bytes);
   
   free(data);
   
   return 0;
}
#endif

void stack_dump(const Stack *const p_stack, const char *const file, const int line)
{
   printf("\n"
          "=======================================================\n");
   
   printf("Stack address:         %#p\n"
          "Min capacity:          %zu\n"
          "Capacity:              %zu\n"
          "Size:                  %zu\n"
          #ifdef STK_CANARY_PROTECT
          "Bytes:                 %zu\n"
          #endif
          "Storage address:       %#p\n",
          p_stack,
          p_stack->minCapacity,
          p_stack->capacity,
          p_stack->size,
          #ifdef STK_CANARY_PROTECT
          p_stack->bytes,
          #endif
          p_stack->storage);

#ifdef STK_CANARY_PROTECT
   printf("-------------------------------------------------------\n");
   printf("Stack left  canary:    %zx\n"
          "Stack right canary:    %zx\n",
          p_stack->leftCanary,
          p_stack->rightCanary);
   
   if (p_stack->storage)
      printf("\n"
             "Storage left  canary:  %zx\n"
             "Storage right canary:  %zx\n",
             *(canary_t *)find_storage_leftCanary(p_stack->storage),
             *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes));
#endif

#ifdef STK_HASH_PROTECT
   hash_t verifyHash = 0;
   calc_stack_hash(p_stack, &verifyHash);
   
   printf("-------------------------------------------------------\n");
   printf("Stack  hash:           %zu\n"
          "Verify hash:           %zu\n",
          p_stack->hash,
          verifyHash);
#endif
   
   if (p_stack->storage)
   {
      printf("-------------------------------------------------------\n");
      
      static element_t *poison = nullptr;
      
      if (poison == nullptr)
      {
         poison = (element_t *)malloc(sizeof(element_t));
         
         memset(poison, STK_POISON, sizeof(element_t));
      }
      
      element_t *p_element     = nullptr;
      element_t *p_lastElement = nullptr;

      #ifdef STK_CANARY_PROTECT
            p_element     = (element_t *)((canary_t *)find_storage_leftCanary(p_stack->storage) + 1);
            p_lastElement = (element_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) - 1;
      #else
            p_element     = (element_t *)p_stack->storage;
            p_lastElement = (element_t *)p_stack->storage + p_stack->capacity - 1;
      #endif
      
      for (size_t i = 0; p_element <= p_lastElement; p_element++, i++)
      {
         printf("Container[%4zu]:       ", i);
         
         if (*p_element == *poison)
            printf("poison\n");
         else
            printf("%" SPECIFIER "\n", *p_element);
      }
   }
   printf("=======================================================\n");
}
#endif