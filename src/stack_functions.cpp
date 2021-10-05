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



static StatementResult is_stack_nullptr(const Stack *const p_stack)
{
   if (p_stack == nullptr)
      return STK_ERROR__NULLPTR;
   
   return STK_SUCCESS;
}

void stack_dump(const Stack *const p_stack, const char *const file, const int line )
{
   const char OK[] = "ok";
   const char BAD[] = "bad";
   
   bool is_address_ok = p_stack != nullptr;
   bool is_size_ok = p_stack->size <= p_stack->capacity;
   bool is_capacity_ok = p_stack->capacity >= p_stack->size;
   bool is_elements_ok = p_stack->elements != nullptr;
   
   char *const p_data = (char *)calloc(500 + p_stack->size * 3, sizeof(char));
   
   strcpy( p_data, "Stack location: %s (%d)\n"
                   "| address:   %16p %s |\n"
                   "| size:      %16zu %s |\n"
                   "| capacity:  %16zu %s |\n"
                   "| elements:  %16p %s |\n"
                   "| data:      [ " );

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
          "| address:   %16p %s |\n"
          "| size:      %16zu %s |\n"
          "| capacity:  %16zu %s |\n"
          "| elements:  %16p %s |\n",
          file, line,
          p_stack, is_address_ok ? OK : BAD,
          p_stack->size, is_size_ok ? OK : BAD,
          p_stack->capacity, is_capacity_ok ? OK : BAD,
          p_stack->elements, is_elements_ok ? OK : BAD);
   
   for (size_t i = 0; i < p_stack->size; i++)
      printf("|| %zu:\t\t%13" STK_SPECIFIER "   ||\n", i, p_stack->elements[i]);
}

static size_t calc_stack_capacity(const size_t minCapacity)
{
   if (minCapacity <= STK_SECTION_CAPACITY)
      return STK_SECTION_CAPACITY;
   
   return ( (minCapacity - 1) / STK_SECTION_CAPACITY + 1 ) * STK_SECTION_CAPACITY;
}

static bool is_stack_inited(Stack* p_stack)
{
   return p_stack->size != 0 || p_stack->capacity != 0 || p_stack->elements != nullptr;
}

StatementResult stack_init(Stack *const p_stack, const size_t minInitCapacity)
{
   if (p_stack == nullptr)
      return STK_ERROR__NULLPTR;
   
   if (is_stack_inited(p_stack))
      return STK_ERROR__REINITIALIZATION;
   
   const size_t     initCapacity = calc_stack_capacity(minInitCapacity);
   element_t *const elements     = (element_t *)calloc(initCapacity, sizeof(element_t));

   if (elements == nullptr)
      return STK_ERROR__UNALLOCATED_MEMORY;
   
   p_stack->size     = 0;
   p_stack->capacity = initCapacity;
   p_stack->elements = elements;
   
   return STK_SUCCESS;
}

StatementResult stack_push( Stack *const p_stack, const element_t element )
{
   if (p_stack == nullptr)
      return STK_ERROR__NULLPTR;
   
   if (p_stack->elements == nullptr)
      return STK_ERROR__ELEMENTS_NULLPTR;
   
   if (p_stack->size == p_stack->capacity) {
      p_stack->capacity += STK_SECTION_CAPACITY;
      p_stack->elements  = (element_t *)realloc( (void *)p_stack->elements, p_stack->capacity );
      
      if (p_stack->elements == nullptr)
         return STK_ERROR__UNALLOCATED_MEMORY;
      
      for (size_t i = 1; i < STK_SECTION_CAPACITY; i++)
         p_stack->elements[p_stack->size + i] = STK_INIT_DATA;
   }
   
   p_stack->elements[p_stack->size] = element;
   p_stack->size += 1;
   
   return STK_SUCCESS;
}

element_t stack_pop(Stack *const p_stack)
{
   if (p_stack == nullptr)
      return 0;
   
   if (p_stack->elements == nullptr)
      return 0;
   
   if (p_stack->size == 0)
      return 0;
   
   
   const element_t  element  = p_stack->elements[p_stack->size - 1];
   const size_t     capacity = calc_stack_capacity(p_stack->size - 1);
   
   if (capacity != p_stack->capacity) {
      p_stack->elements = (element_t *)realloc( p_stack->elements, p_stack->capacity * sizeof(element_t) );
      
      if (p_stack->elements == nullptr)
         return STK_ERROR__UNALLOCATED_MEMORY;
   }
   else
      p_stack->elements[p_stack->size - 1] = STK_INIT_DATA;
   
   p_stack->size -= 1;
   p_stack->capacity = capacity;
   
   return element;
}
