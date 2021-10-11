#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../include/stack_functions.h"



/*!
 * @brief Typedef for stack element's data type
 */
typedef stk_element_t element_t;

/*!
 * @brief Typedef for result of statement with stack
 */
typedef StackStatementResult StatementResult;




void stack_dump( const Stack *const p_stack, const char *const file, const int line )
{
   const char OK[] = "ok";
   const char BAD[] = "bad";
   
   bool is_address_ok = p_stack != nullptr;
   bool is_size_ok = p_stack->size <= p_stack->capacity;
   bool is_capacity_ok = p_stack->capacity >= p_stack->size && p_stack->capacity >= p_stack->minCapacity;
   bool is_minCapacity_ok = p_stack->capacity >= p_stack->minCapacity;
   bool is_elements_ok = p_stack->elements != nullptr;
   
   char *const p_data = (char *)calloc(500 + p_stack->size * 3, sizeof(char));
   
   strcpy( p_data, "Stack location: %s (%d)\n"
                   "| address:     %16p %s |\n"
                   "| size:        %16zu %s |\n"
                   "| capacity:    %16zu %s |\n"
                   "| minCapacity: %16zu %s |\n"
                   "| elements:    %16p %s |\n"
                   "| data:        [ " );

   const size_t len = strlen(p_data);
   for (size_t i = 0; i < p_stack->size; i++) {
      p_data[len + i * 3] = '%';
      p_data[len + i * 3 + 1] = 'd';
      p_data[len + i * 3 + 2] = ' ';
   }
   p_data[len + 0 + p_stack->size * 3] = ']';
   p_data[len + 1 + p_stack->size * 3] = '\n';
   p_data[len + 2 + p_stack->size * 3] = '\0';
   
   printf("Stack location: %s (%d)\n"
          "| address:     %16p %s |\n"
          "| size:        %16zu %s |\n"
          "| capacity:    %16zu %s |\n"
          "| minCapacity: %16zu %s |\n"
          "| elements:    %16p %s |\n",
          file, line,
          p_stack, is_address_ok ? OK : BAD,
          p_stack->size, is_size_ok ? OK : BAD,
          p_stack->capacity, is_capacity_ok ? OK : BAD,
          p_stack->minCapacity, is_minCapacity_ok ? OK : BAD,
          p_stack->elements, is_elements_ok ? OK : BAD);
   
   for (size_t i = 0; i < p_stack->size; i++)
      printf("|| %zu:\t\t  %13" STK_SPECIFIER "   ||\n", i, p_stack->elements[i]);
}

static size_t stack_reduce_capacity( const size_t capacity, const size_t minCapacity = 0 )
{
   if (capacity <= minCapacity)
      return minCapacity;
   
   size_t newCapacity = 2;
   
   while (newCapacity < capacity)
   {
      if (newCapacity == 0)
         newCapacity = SIZE_MAX;
      
      newCapacity *= 2;
   }
   
   return newCapacity;
}

static bool is_stack_inited(Stack* p_stack)
{
   return p_stack->size != 0 ||
          p_stack->capacity != 0 ||
          p_stack->minCapacity != 0 ||
          p_stack->elements != nullptr;
}

static bool is_stack_invalid(const Stack *const p_stack)
{
   if (p_stack->elements == nullptr && p_stack->size == 0)
      return false;
   
   if (p_stack->size <= p_stack->capacity)
      return false;
   
   if (p_stack->minCapacity <= p_stack->capacity)
      return false;
   
   return true;
}

StatementResult stack_init( Stack *const p_stack,
                            const size_t desiredMinCapacity,
                            const size_t desiredInitCapacity )
{
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;
   
   if (is_stack_inited(p_stack))
      return STK_ERROR__STACK_REINITIALIZATION;
   
   const size_t  minCapacity  = stack_reduce_capacity(desiredMinCapacity);
   size_t        initCapacity = stack_reduce_capacity(desiredInitCapacity, minCapacity);
   element_t    *elements     = nullptr;
   
   if (initCapacity > 0)
   {
      elements = (element_t *)calloc(initCapacity, sizeof(element_t));
      
      if (elements == nullptr)
         return STK_ERROR__UNALLOCATED_MEMORY;
   }

   
   p_stack->size        = 0;
   p_stack->capacity    = initCapacity;
   p_stack->minCapacity = minCapacity;
   p_stack->elements    = elements;
   
   return STK_SUCCESS;
}

StatementResult stack_push(Stack *const p_stack, const element_t element)
{
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;

   if (is_stack_invalid(p_stack))
      return STK_ERROR__STACK_MISUSE;
   
   const size_t           prevSize     = p_stack->size;
   const size_t           newSize      = prevSize + 1;
   const size_t           prevCapacity = p_stack->capacity;
   size_t                 newCapacity  = prevCapacity;
   const element_t *const prevElements = p_stack->elements;
   element_t             *newElements  = p_stack->elements;
   
   if (prevSize == SIZE_MAX)
      return STK_ERROR__STACK_SIZE_MAX;
   
   if (newSize == 1)
   {
      newCapacity = stack_reduce_capacity(prevSize + 1, p_stack->minCapacity);
      newElements = (element_t *)calloc(newCapacity, sizeof(element_t));
   
      if (newElements == nullptr) {
         return STK_ERROR__UNALLOCATED_MEMORY;
      }
   
      for (size_t i = prevCapacity; i < newCapacity; i++)
         newElements[i] = STK_POISON;
   }
   else if (newSize > prevCapacity)
   {
      newCapacity = stack_reduce_capacity(prevSize + 1, p_stack->minCapacity);
      newElements = (element_t *)realloc((void *)prevElements, newCapacity * sizeof(element_t));
      
      if (newElements == nullptr)
         return STK_ERROR__UNALLOCATED_MEMORY;
   
      for (size_t i = prevCapacity; i < newCapacity; i++)
         newElements[i] = STK_POISON;
   }
   
   newElements[newSize - 1] = element;
   
   p_stack->size      = newSize;
   p_stack->capacity  = newCapacity;
   p_stack->elements  = newElements;
   
   return STK_SUCCESS;
}

StatementResult stack_pop(Stack *const p_stack, element_t *const p_output)
{
  if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;
   
   if (is_stack_invalid(p_stack))
      return STK_ERROR__STACK_MISUSE;
   
   if (p_stack->size == 0)
      return STK_ERROR__STACK_EMPTY;
   
   if (p_output == nullptr)
     return STK_ERROR__OUTPUT_NULLPTR;
   
   
   const size_t           prevSize     = p_stack->size;
   const size_t           newSize      = prevSize - 1;
   const size_t           prevCapacity = p_stack->capacity;
   size_t                 newCapacity  = prevCapacity;
   const element_t *const prevElements = p_stack->elements;
   element_t             *newElements  = p_stack->elements;
   const element_t        element      = prevElements[prevSize - 1];
   
   newCapacity = stack_reduce_capacity(prevSize - 1, p_stack->minCapacity);
   
   if (newCapacity != prevCapacity)
   {
      newElements = (element_t *)realloc((void *)prevElements, newCapacity * sizeof(element_t));
   
      if (newElements == nullptr && newCapacity != 0)
         return STK_ERROR__UNALLOCATED_MEMORY;
   }
   else
      newElements[prevSize - 1] = STK_POISON;
   
   *p_output = element;
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   p_stack->elements = newElements;
   
   return STK_SUCCESS;
   
}

StatementResult stack_kill(Stack *const p_stack)
{
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;
   
   free((void *)p_stack->elements);
   
   p_stack->size = 0;
   p_stack->capacity = 0;
   p_stack->minCapacity = 0;
   p_stack->elements = nullptr;
   
   return STK_SUCCESS;
}