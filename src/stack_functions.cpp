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
      return STK_ERROR__STACK_NULLPTR;
   
   return STK_SUCCESS;
}

void stack_dump( const Stack *const p_stack, const char *const file, const int line )
{
   const char OK[] = "ok";
   const char BAD[] = "bad";
   
   bool is_address_ok = p_stack != nullptr;
   bool is_size_ok = p_stack->size <= p_stack->capacity && p_stack->size <= p_stack->minCapacity;
   bool is_capacity_ok = p_stack->capacity >= p_stack->size && p_stack->capacity >= p_stack->minCapacity;
   bool is_minCapacity_ok = p_stack->capacity >= p_stack->minCapacity && p_stack->size <= p_stack->minCapacity;
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

static size_t calc_stack_capacity( const size_t capacity, const size_t minCapacity )
{
   if (capacity <= minCapacity)
      return minCapacity;
   
   return ((capacity - 1) / STK_SECTION_CAPACITY + 1 ) * STK_SECTION_CAPACITY;
}

static size_t calc_stack_minCapacity(const size_t minCapacity)
{
   if (minCapacity <= STK_MIN_CAPACITY)
      return STK_MIN_CAPACITY;
   
   return ((minCapacity - 1) / STK_MIN_CAPACITY + 1 ) * STK_MIN_CAPACITY;
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
   if ( p_stack->elements != nullptr &&
        p_stack->size <= p_stack->capacity &&
        p_stack->minCapacity <= p_stack->capacity &&
        STK_MIN_CAPACITY <= p_stack->capacity &&
        STK_MIN_CAPACITY <= p_stack->minCapacity )
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
   
   const size_t     minCapacity  = calc_stack_minCapacity(desiredMinCapacity);
   const size_t     initCapacity = calc_stack_capacity( desiredInitCapacity, minCapacity );
   element_t *const elements     = (element_t *)calloc( initCapacity, sizeof(element_t) );

   if (elements == nullptr)
      return STK_ERROR__UNALLOCATED_MEMORY;
   
   p_stack->size        = 0;
   p_stack->capacity    = initCapacity;
   p_stack->minCapacity = minCapacity;
   p_stack->elements    = elements;
   
   return STK_SUCCESS;
}

StatementResult stack_push( Stack *const p_stack, const element_t element )
{
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;

   if (is_stack_invalid(p_stack))
      return STK_ERROR__STACK_MISUSE;
   
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

StatementResult stack_pop( Stack *const p_stack, element_t *const p_output )
{
   if (p_stack == nullptr)
      return STK_ERROR__STACK_NULLPTR;
   
   if (is_stack_invalid(p_stack))
      return STK_ERROR__STACK_MISUSE;
   
   if (p_stack->size == 0)
      return STK_ERROR__STACK_EMPTY;
   
   if (p_output == nullptr)
     return STK_ERROR__OUTPUT_NULLPTR;
   
   *p_output = p_stack->elements[p_stack->size - 1];
   
   const size_t capacity = calc_stack_capacity( p_stack->size - 1, p_stack->minCapacity );
   
   if (capacity != p_stack->capacity) {
      p_stack->elements = (element_t *)realloc( p_stack->elements, p_stack->capacity * sizeof(element_t) );
      
      if (p_stack->elements == nullptr)
         return STK_ERROR__UNALLOCATED_MEMORY;
   }
   else
      p_stack->elements[p_stack->size - 1] = STK_INIT_DATA;
   
   p_stack->size    -= 1;
   p_stack->capacity = capacity;
   return STK_SUCCESS;
   
}