#include <cstdio>
#include <cstdlib>
#include "include/stack.h"
#include "prototyping/prototyping.h"

int main() {
   Stack stack = {};

   stack_init(&stack);
   
   for (size_t i = 0; i < 20; i++)
   {
      stack_push(&stack, i);
   }

   stk_element_t j = 100;
   stack_dump(&stack, __FILE__, __LINE__);
   
   for (size_t i = 0; i < 10; i++)
   {
      stack_pop(&stack, &j);
   }
   
   stack_destroy(&stack);
   
   stack_dump(&stack, __FILE__, __LINE__);
   
   return 0;
}
