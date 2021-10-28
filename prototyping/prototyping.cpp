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
   
   while (byte < lastByte)
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


int print_data_hard(const void* const data, const size_t data_bytes,
                    const size_t _columns = 4, const size_t _cell_bytes = 4, const size_t _tabs = 0)
{
   if (data && data_bytes)
   {
//      printf("BYTES: %zu", data_bytes);
      
      const size_t        MAX_TABS = 6;
      const size_t        MAX_COLUMNS = 8;
      const size_t        MAX_CELL_BYTES = 6;
      
      const size_t tabs = _tabs <= MAX_TABS ? _tabs : MAX_TABS;
      char         *const tabs_str = (char *)calloc(tabs + 1, sizeof(char)); /// ?error processing

//      if (tabs_str == nullptr)
//         return 1;
      
      const size_t columns = _columns <= MAX_COLUMNS ? _columns : MAX_COLUMNS;
      const size_t cell_bytes = _cell_bytes <= MAX_CELL_BYTES ? _cell_bytes : MAX_CELL_BYTES;
      const size_t fullRows  = data_bytes / (columns * cell_bytes);
      const size_t fullCells = data_bytes / cell_bytes;
      const size_t rows = fullRows + (data_bytes % (columns * cell_bytes) != 0);
      const size_t cells = fullCells + (data_bytes % cell_bytes != 0);
      size_t       row       = 0;
      size_t       cell      = 0;
      size_t       byte      = 0;
      size_t       iterator  = 0;
      
      for (size_t i = 0; i < tabs; i++)
         tabs_str[i] = '\t';
      tabs_str[tabs] = '\0';
      
      printf("\n%s-------------------------------------------------------------\n", tabs_str);
      
      for (; row < fullRows; row++)
      {
         printf("%s", tabs_str);
         
         for (iterator = 0; iterator < columns; cell++, iterator++)
            printf("   %02zx - %02zx \t", cell * cell_bytes, (cell + 1) * cell_bytes - 1);
         
         printf("\n%s|", tabs_str);
         
         for (iterator = 0; iterator < cell_bytes * columns; byte++, iterator++)
         {
            if (iterator % 4 == 0 && iterator)
               printf("\t|");
            
            printf("%02x|", *((unsigned char *)data + byte));
         }
         
         if (row + 1 != rows)
            printf("\n%s-------------------------------------------------------------\n", tabs_str);
      }
      
      if (fullRows != rows)
      {
         printf("%s", tabs_str);
         
         for (; cell < fullCells; cell++)
            printf("   %02zx - %02zx \t", cell * cell_bytes, (cell + 1) * cell_bytes - 1);
         
         if (fullCells != cells)
         {
            if (data_bytes - byte == 1)
               printf("   %02zx", byte);
            else
            {
               printf("   %02zx - %02zx", cell * cell_bytes, data_bytes - 1);
            }
         }
         
         printf("\n%s|", tabs_str);
         
         for (iterator = 0; byte < data_bytes; byte++)
         {
            if (byte % 4 == 0 && iterator++)
               printf("\t|");
            
            printf("%02x|", *((unsigned char *)data + byte));
         }
      }
      
      printf("\n%s-------------------------------------------------------------\n", tabs_str);
   }
}

void print_data(const void* const data, const size_t data_bytes,
                const size_t _columns = 8, const size_t _cell_bytes = 4, const size_t _tabs = 0)
{
   if (data && data_bytes)
   {
//      printf("BYTES: %zu", data_bytes);
      
      const size_t        MAX_TABS = 6;
      const size_t        MAX_COLUMNS = 8;
      const size_t        MAX_CELL_BYTES = 6;
      
      const size_t tabs = _tabs <= MAX_TABS ? _tabs : MAX_TABS;
      char         *const tabs_str = (char *)calloc(tabs + 1, sizeof(char)); /// ?error processing
      
      const size_t columns = _columns <= MAX_COLUMNS ? _columns : MAX_COLUMNS;
      const size_t cell_bytes = _cell_bytes <= MAX_CELL_BYTES ? _cell_bytes : MAX_CELL_BYTES;
      const size_t fullRows  = data_bytes / (columns * cell_bytes);
      const size_t fullCells = data_bytes / cell_bytes;
      const size_t rows = fullRows + (data_bytes % (columns * cell_bytes) != 0);
      const size_t cells = fullCells + (data_bytes % cell_bytes != 0);
      size_t       row       = 0;
      size_t       cell      = 0;
      size_t       byte      = 0;
      size_t       iterator  = 0;
      
      for (size_t i = 0; i < tabs; i++)
         tabs_str[i] = '\t';
      tabs_str[tabs] = '\0';
      
      printf("\n%s-----------------------------------------------------------------------------------------\n", tabs_str);
      
      for (; row < fullRows; row++)
      {
         printf("%s|", tabs_str);
         
         for (iterator = 0; iterator < cell_bytes * columns; byte++, iterator++)
         {
            if (iterator % 4 == 0 && iterator)
               printf(" |");
            
            printf("%02x|", *((unsigned char *)data + byte));
         }
         
         if (row + 1 != rows)
            printf("\n%s", tabs_str);
      }
      
      if (fullRows != rows)
      {
         printf("%s|", tabs_str);
         
         for (iterator = 0; byte < data_bytes; byte++)
         {
            if (byte % 4 == 0 && iterator++)
               printf(" |");
            
            printf("%02x|", *((unsigned char *)data + byte));
         }
      }
      
      printf("\n%s-----------------------------------------------------------------------------------------\n", tabs_str);
   }
}
