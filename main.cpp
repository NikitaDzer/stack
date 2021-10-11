#define DEV
// #define PRODUCTION

#include <cstdio>
#include "include/stack.h"
#include "include/stack_functions.h"


int main() {
   Stack stack = {};
   stack_init(&stack);
   int a = 1;
   stack_push(&stack, 1);
   stack_pop(&stack, &a);
   stack_pop(&stack, &a);
   printf("%d\n", a);
   
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   stack_dump(&stack, __FILE__, __LINE__);

   return 0;
}
