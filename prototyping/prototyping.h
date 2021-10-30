#ifndef PROTOTYPING_H
#define PROTOTYPING_H

#include <cstddef>

void check_bites(const size_t number);

void test_hash(const size_t input_size, const size_t tests_number);

void print_data(const void* const data, const size_t data_bytes,
                const size_t _columns = 8, const size_t _cell_bytes = 4, const size_t _tabs = 0);

#endif // PROTOTYPING_H
