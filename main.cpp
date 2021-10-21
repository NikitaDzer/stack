#define DEV
// #define PRODUCTION

#include <cstdio>
#include <cstdlib>
#include "include/stack.h"
#include "include/stack_functions.h"


int main() {
   Stack stack = {};
   
   stack_init(&stack);
   stack_push(&stack, 100);
   stack_push(&stack, 200);
   stack_push(&stack, 300);
   
   int a = 0;
   stack_pop(&stack, &a);
   stack_pop(&stack, &a);
   
   stack_dump(&stack, __FILE__, __LINE__);
   
   stack_destroy(&stack);
   
   return 0;
}
