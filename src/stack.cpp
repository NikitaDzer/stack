#include <cstdio>
#include <cstddef>
#include <cstdlib>

#include "../include/stack.h"

/*!
 * @brief Typedef for stack element's data type
 */
typedef int element_type;

/*!
 * @brief Typedef for stack error code
 */
typedef int error_code;






/// functions ->

static error_code is_stack_nullptr(Stack *p_stack)
{
   if (p_stack == nullptr)
      return STACK__ERROR_NULLPTR;
   
   return STACK__SUCCESS;
}


void print_stack( Stack stack )
{
   printf("Stack:\n"
          "\tsize: %zu\n"
          "\tcapacity: %zu\n\n"
          "\tdump:\n"
          "\t\telements: %p\n",
          stack.size, stack.capacity, stack._dump.elements);
}


static size_t calc_stack_realSize( size_t stack_size )
{
   if (stack_size == 0)
      return 0;
   
   return (stack_size >> 4 << 4) + 16;
}

static size_t calc_stack_sections_number( size_t stack_size )
{
   if (stack_size == 0)
      return 0;
   
   return (stack_size >> 4) + 1;
}



error_code stack_init( Stack *p_stack, size_t stack_size, element_type initData )
{
   if (is_stack_nullptr( p_stack ))
      return STACK__ERROR_NULLPTR;
   
   if (p_stack->_dump.is_initialed)
      return STACK__ERROR_REINITIALIZATION;
   
   p_stack->size      = stack_size;
   p_stack->capacity  = stack_size;
   
   p_stack->_dump.is_initialed        = true;
   p_stack->_dump.elements            = nullptr;
   p_stack->_dump.initData            = initData;
   p_stack->_dump.realSize            = calc_stack_realSize( stack_size );
   p_stack->_dump.sections_number     = calc_stack_sections_number( stack_size );
   p_stack->_dump.usedSections_number = 0;
   
   return STACK__SUCCESS;
}

element_type stack_push(Stack* const p_stack, const element_type element)
{
   if (is_stack_nullptr(p_stack))
      return STACK__ERROR_NULLPTR;
   
   
}


/// <- functions