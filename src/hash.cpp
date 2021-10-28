#include "../include/hash.h"

hash_t qhash(const void *const data, const size_t data_bytes)
{
   const char       *byte     = (char *)data;
   const char *const lastByte = byte + data_bytes;
   hash_t            hash     = 0xDED007;
   
   while (byte < lastByte)
      hash = ((hash << 0x8) + (hash >> 0x8)) ^ *byte++;
   
   return hash;
}