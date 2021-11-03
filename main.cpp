#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "include/stack.h"
#include "include/dump.h"

int main()
{
   clock_t start = clock();
   
   Stack stack = {};
   
//   stack_debug(&stack);
   
   Stack stack1 = {};
   
   stack_init(&stack, 0);
   stack_init(&stack1, 0);
   
   stk_element_t element = 10;
   
   stack_push(&stack, 100);

   stack.size = 10;
   
   stack_push(&stack, 200);
   stack_pop(&stack, nullptr);
   
   stack_destroy(&stack);
   
   
   stack_push(&stack, 300);
   stack_pop(&stack, nullptr);
   
   printf("\nTime elapsed: %.5lf\n", (double)(clock() - start) / CLOCKS_PER_SEC);
   
   return 0;
}
