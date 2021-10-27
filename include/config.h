#ifndef STACK_CONFIG_H
#define STACK_CONFIG_H

#include <cstddef>

//#define STK_PRODUCTION
#define STK_DEBUG

//#define STK_UNPROTECT
#define STK_CANARY_PROTECT
//#define STK_HASH_PROTECT

#define STK_ELEMENT_SPECIFIER "d"
typedef int stk_element_t;

const size_t STK_MIN_CAPACITY = 2;
const size_t STK_MAX_CAPACITY = (SIZE_MAX >> 1) + 1;

#endif // STACK_CONFIG_H
