#define DEV
// #define PRODUCTION

#include "include/stack.h"



int main() {
   Stack stack = {};
   stack_init(&stack);
   
   
   print_stack(stack);
   
  // stack_push(&stack, 210);
   
   return 0;
}
