#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "include/stack.h"
#include "prototyping/prototyping.h"

int main() {
   
   clock_t start = clock();

   Stack stack = {};

   stack_init(&stack);
   
   
   
   stk_element_t j = 100;
   
   for (size_t i = 0; i < 1000000; i++) {
      stack_push(&stack, i);
      stack_push(&stack, i);
      stack_pop(&stack, &j);
      stack_pop(&stack, &j);
   }
   
//   stack_dump(&stack, __FILE__, __LINE__);

   
   stack_destroy(&stack);
   
//   stack_dump(&stack, __FILE__, __LINE__);
   
   
   clock_t stop = clock();
   
   printf("\nTime elapsed: %.5f\n", (double) (stop - start) / CLOCKS_PER_SEC);
   
   return 0;
}
