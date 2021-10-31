#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "include/stack.h"
#include "prototyping/prototyping.h"
#include "include/dump.h"

int main()
{
   clock_t start = clock();

   
   
   Stack stack = {};
   
   Stack stack1 = {};
   
   stack_init(&stack, SIZE_MAX);
   stack_init(&stack, 2);
   stack_init(&stack, 2);
   stack_init(&stack, 2);
//   stack_init(&stack1, SIZE_MAX);
   
//   stack_log(&stack, CALLER_STACK_INIT, __FILE__, __LINE__, StackStatementDetails::STACK_NULLPTR);
   
//
//   stack.storage = nullptr;
//   stack_push(&stack, 100);
   
   
   stk_element_t i = 0;
   
   stack_push(&stack, 200);
   stack_push(&stack, 300);
   
   
   stack_pop(&stack, &i);
   stack.storage = nullptr;
   stack.bytes = 0;
   
   stack_destroy(&stack);
   stack_init(&stack);
   
   stack_push(&stack, 400);
   stack_pop(&stack, &i);
   
   printf("%" SPECIFIER, i);
   

//   stack_dump(&stack1);

//   stack_log(&stack);
   stack_destroy(&stack);
   stack_dump(&stack);
   
   
//   stack_dump(&stack, __FILE__, __LINE__);
//


   printf("\nTime elapsed: %.5lf\n", (double)(clock() - start) / CLOCKS_PER_SEC);
   
   return 0;
}
