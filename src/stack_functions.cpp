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
         newCapacity = 0;
      
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

static int stack_change_capacity(Stack *const p_stack, const bool is_increment)
{
   // --align declarations
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   if (is_increment)
   {
      if (prevSize == STK_MAX_CAPACITY)
         return 1;
   
      newSize = prevSize + 1;
   
      if (newSize <= minCapacity)
         newCapacity = minCapacity;
      else
      {
         if (newSize <= prevCapacity)
            newCapacity = prevCapacity;
         else
            newCapacity = prevCapacity * 2;
      }
   }
   else
   {
      if (prevSize == 0)
         return 1;
   
      newSize = prevSize - 1;
   
       if (newSize <= minCapacity)
         newCapacity = minCapacity;
       else
       {
          if (newSize * 2 <= prevCapacity) // --optimization
             newCapacity = prevCapacity / 2; // --optimization
          else
             newCapacity = prevCapacity;
       }
   }
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
   return 0;
}

static element_t* get_lastElement_ptr(const element_t *const storage, const size_t size) //? maybe fix fn name
{
   return (element_t *)((char *)storage + sizeof(STK_COCK_HEN_SHIELD) + sizeof(element_t) * (size - 1)); //? correctly
}

static int stack_realloc(Stack *const p_stack) //! make poison filler
{
   const size_t           size         = p_stack->size;
   const size_t           capacity     = p_stack->capacity;
   const element_t *const prevStorage = p_stack->elements;
   element_t             *newStorage  = nullptr;
   
   if (capacity == 0)
      free((void *)prevStorage);
   else
   {
      const size_t cock_size_in_bytes      = sizeof(STK_COCK_HEN_SHIELD),
                   shield_size_in_bytes    = sizeof(STK_COCK_HEN_SHIELD) * 2,
                   container_size_in_bytes = sizeof(element_t) * capacity;
      size_t storage_size_in_bytes = cock_size_in_bytes;
   
      while (storage_size_in_bytes < container_size_in_bytes)
         storage_size_in_bytes += cock_size_in_bytes;
   
      storage_size_in_bytes += shield_size_in_bytes;
   
      newStorage = (element_t *)realloc((void *)prevStorage, storage_size_in_bytes);
      
      if (newStorage == nullptr)
         return 1;
      
//      element_t *const p_lastElement = get_lastElement_ptr(newStorage, size);
//      if (*p_lastElement != STK_POISON)
//         for (element_t *p_element = p_lastElement; p_element - p_lastElement < capacity - size; p_element++) //! opt
//            *p_element = STK_POISON;
//
   }
   
   p_stack->elements = newStorage;
   
   return 0;
}

StatementResult stack_init(Stack *const p_stack, const size_t desiredMinCapacity)
{
   if (p_stack == nullptr)
      return StatementResult::STACK_NULLPTR;
   
   if (is_stack_inited(p_stack))
      return StatementResult::STACK_REINITIALIZATION;
   
   size_t     minCapacity = 0;
   element_t *elements    = nullptr;
   
   if (desiredMinCapacity == 0)
      minCapacity = 0;
   else
   {
      if (desiredMinCapacity > STK_MAX_CAPACITY)
         minCapacity = STK_MAX_CAPACITY;
      else
      {
         minCapacity = 2;
         
         while (minCapacity < desiredMinCapacity)
            minCapacity *= 2;
      }
   }
   
   if (minCapacity > 0)
   {
      elements = (element_t *)calloc(minCapacity, sizeof(element_t));
   
      if (elements == nullptr)
         return StatementResult::UNALLOCATED_MEMORY;
   }
   
   p_stack->cock        = STK_COCK_HEN_SHIELD;
   p_stack->size        = 0;
   p_stack->capacity    = minCapacity;
   p_stack->minCapacity = minCapacity;
   p_stack->elements    = elements;
   p_stack->hen         = STK_COCK_HEN_SHIELD;
   
   return StackStatementResult::SUCCESS;
}

StatementResult stack_push(Stack *const p_stack, const element_t element)
{
   if (p_stack == nullptr)
      return StatementResult::STACK_NULLPTR;

   if (is_stack_invalid(p_stack))
      return StatementResult::STACK_MISUSE;

   if (stack_change_capacity(p_stack, true))
      return StatementResult::STACK_EXCEEDED_MAX_CAPACITY;
   
   if (stack_realloc(p_stack))
      return StatementResult::STACK_NON_UPDATED_ELEMENTS;
   
   p_stack->elements[p_stack->size - 1] = element;
   
   return StatementResult::SUCCESS;
}


StatementResult stack_pop(Stack *const p_stack, element_t *const p_output)
{
/*  if (p_stack == nullptr)
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
  */
   return StatementResult::SUCCESS;
   
}

StatementResult stack_kill(Stack *const p_stack)
{
   if (p_stack == nullptr)
      return StatementResult::STACK_NULLPTR;
   
   free((void *)p_stack->elements);
   
   p_stack->cock = 0;
   p_stack->size = 0;
   p_stack->capacity = 0;
   p_stack->minCapacity = 0;
   p_stack->elements = nullptr;
   p_stack->hen = 0;
   
   return StatementResult::SUCCESS;
}

static StatementResult stack_inspector(const Stack *const p_stack) {
   /*
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;
   
   if (p_stack->size > p_stack->capacity)
      return STK_ERROR__STACK_NULLPTR;
   
   if (p_stack->elements == nullptr && p_stack->size == 0) {
      return STK_ERROR__STACK_NULLPTR;
   }
   
   
   if (p_stack->minCapacity <= p_stack->capacity)
      return STK_ERROR__STACK_NULLPTR;
*/
}