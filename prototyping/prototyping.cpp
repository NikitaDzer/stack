#include <cstdio>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include "prototyping.h"


uint64_t qhash(const void *const data, const size_t data_bytes)
{
   const char       *byte     = (char *)data;
   const char *const lastByte = byte + data_bytes;
   uint64_t          hash     = 0xDED007;
   
   while (byte <= lastByte)
      hash = ((hash << 0x8) + (hash >> 0x8)) ^ *byte++;
   
   // hash = (hash << (byte & 0xff | 1) | hash << (byte & 0xf) | hash >> (byte & 0xf) ^ hash)
   //       ^ ~(byte << (byte & hash & 0xf) ^ hash >> 1);
   
   return hash;
}


void test_hash(const size_t input_size, const size_t tests_number)
{
   srand(time(0));
   
   size_t *numbers = (size_t *)malloc(sizeof(size_t) * input_size);
   size_t *hash_sums = (size_t *)malloc(sizeof(size_t) * tests_number);
   size_t *sorted_hash_sums = (size_t *)malloc(sizeof(size_t) * tests_number);
   size_t hash_sum     = 0,
          size         = 0,
          sorted_size  = 0;
   
   for (size_t i = 0; i < tests_number; i++)
   {
      size = rand() % (input_size - 10) + 10;
      
      for (size_t j = 0; j < size; j++)
         numbers[j] = rand();
   
      hash_sums[i] = qhash(numbers, size);
   }
   
   
   sorted_hash_sums[0] = hash_sums[0];
   sorted_size++;
   
   for (size_t i = 1, j = 0; i < tests_number; i++)
   {
      hash_sum = hash_sums[i];
      
      for (j = 0; j < sorted_size; j++)
         if (hash_sum == sorted_hash_sums[j])
            break;
            
      if (j == sorted_size)
         sorted_hash_sums[sorted_size++] = hash_sum;
   }
   
//  for (size_t i = 0; i < sorted_size; i++)
      printf("Unique hash_sums: %zu\n", sorted_size);
}

