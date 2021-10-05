#ifndef STACK_H
#define STACK_H

#include <cstddef>

/*!
 * @brief Typedef for stack element's data type
 */
typedef int stack__element_type;

/*!
 * @brief Typedef for stack error code
 */
typedef int stack__error_code;




struct StackDump {
   bool is_initialed;
   size_t realSize;
   size_t sections_number;
   size_t usedSections_number;
   stack__element_type *elements;
   stack__element_type initData;
};


/*!
 * @brief Stack
 */
struct Stack {
   // stack fields ->
   size_t size;
   size_t capacity;
   
   StackDump _dump;
   // <- stack fields
   
   
   
   // stack methods ->
   
   // <- stack methods
};





/// global constants ->

const stack__element_type STACK__DEFAULT_DATA = 'u';

const stack__error_code STACK__ERROR_REINITIALIZATION = 3;
const stack__error_code STACK__ERROR_UNALLOCATED_MEMORY = 2;
const stack__error_code STACK__ERROR_NULLPTR = 1;
const stack__error_code STACK__SUCCESS = 0;

/// <- global constants






/*!
 * @brief Prints information about putted stack
 * @param stack Pointer of stack to be print
 */
void print_stack(Stack stack);


/*!
 * @brief Constructor for new stack
 * @param p_stack   Stack pointer
 * @param size      Stack size
 * @param init_data Stack initial data
 * @return
 */
stack__error_code stack_init(Stack *p_stack,
                             size_t stack_size = 0,
                             stack__element_type initData = STACK__DEFAULT_DATA);


#endif // STACK_H
