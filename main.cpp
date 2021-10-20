#define DEV
// #define PRODUCTION

#include <cstdio>
#include <cstdlib>
#include "include/stack.h"
#include "include/stack_functions.h"


int main() {
   Stack stack = {};
   stack_init(&stack, 10);
   
   int p = 10;
   
   
   stack_push(&stack, 100);
   stack_push(&stack, 500);
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   stack_push(&stack, 100);
   
   stack_dump(&stack, __FILE__, __LINE__);
   

   return 0;
}
