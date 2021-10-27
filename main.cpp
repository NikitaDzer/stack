#define DEV
// #define PRODUCTION

#include <cstdio>
#include <cstdlib>
#include "include/stack.h"
#include "include/stack_methods.h"
#include "prototyping/prototyping.h"

void print(const size_t number)
{
   printf("%zu\n", number);
}

int main() {
   
//   test_hash(15, 10000);
   Stack stack = {};
   

   stack_init(&stack);
   
   
   for (size_t i = 0; i < 10; i++)
   {
      stack_push(&stack, i);
      stack_dump(&stack, __FILE__, __LINE__);
   }

   stk_element_t j = 100;

   for (size_t i = 0; i < 10; i++) {
//      stack_pop(&stack, &j);
//      if (stack_pop(&stack, &j) & StackDetails::BANNED)
//         printf("AaaaaaAAAAAAAAAAAAAAA");
   }
   
   stack_destroy(&stack);
   
   return 0;
}
