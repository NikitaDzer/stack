#ifndef STACK_HASH_H
#define STACK_HASH_H

#include <cstddef>

typedef size_t hash_t;

hash_t qhash(const void *const data, const size_t data_bytes);

#endif // STACK_HASH_H
