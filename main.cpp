#define DEV
// #define PRODUCTION

#include <cstdio>
#include "include/stack.h"
#include "include/stack_functions.h"


int main() {
   Stack stack = {};
   stack_init(&stack);
   
   stack_push(&stack, 100);

   stack_dump(&stack, __FILE__, __LINE__);
   
   printf("%" STK_SPECIFIER, stack_pop(&stack));
   
   return 0;
}
