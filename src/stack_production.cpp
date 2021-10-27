#include <cstdlib>
#include "../include/stack.h"

typedef stk_element_t element_t;

#ifdef STK_PRODUCTION
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
#endif // STK_PRODUCTION