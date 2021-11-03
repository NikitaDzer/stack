#ifndef STACK_HASH_H
#define STACK_HASH_H

#include <cstddef>

typedef size_t hash_t;

hash_t qhash(const void *const buffer, const size_t buffer_bytes);

#endif // STACK_HASH_H
