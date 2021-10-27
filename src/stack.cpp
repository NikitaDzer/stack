#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../include/stack.h"

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


typedef stk_bitmask_t bitmask_t;
typedef stk_element_t element_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
#endif

#ifdef STK_HASH_PROTECT
typedef stk_hash_t    hash_t;
#endif


#ifdef STK_CANARY_PROTECT
static void* find_storage_leftCanary(const void *const storage)
{
   return (void *)storage;
}

static void* find_storage_rightCanary(const void *const storage, const size_t bytes)
{
   if (storage == nullptr)
      return nullptr;
   
   return (void *)((char *)storage + bytes - sizeof(canary_t));
}
#endif


static void* find_firstElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t));
#else
   return (void *)storage;
#endif
}

static void* find_lastElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t) + sizeof(element_t) * (size - 1));
#else
   return (void *)((char *)storage + sizeof(element_t) * (size - 1));
#endif
}

#ifdef STK_HASH_PROTECT
static hash_t quickHash(const void *const data, const size_t data_bytes)
{
   const char       *byte     = (char *)data;
   const char *const lastByte = byte + data_bytes;
   hash_t            hash     = 0xDED007;
   
   while (byte < lastByte)
      hash = ((hash << 0x8) + (hash >> 0x8)) ^ *byte++;
   
   return hash;
}

static int calc_stack_hash(const Stack *const p_stack, hash_t *const p_hash)
{
   const size_t        bytes = p_stack->bytes;
   char         *const data  = (char *)malloc(sizeof(Stack) + bytes);
   
   if (data == nullptr) {
      return 1;
   }
   
   memcpy(data,                 p_stack,          sizeof(Stack));
   memcpy(data + sizeof(Stack), p_stack->storage, bytes);
   memset(data + ((char *)&p_stack->hash - (char *)p_stack), 0, sizeof(size_t));
   
   *p_hash = quickHash(data, sizeof(Stack) + bytes);
   
   free(data);
   
   return 0;
}
#endif

void stack_dump(const Stack *const p_stack, const char *const file, const int line)
{
   printf("minCapacity:  --------   %20zu\n"
          "size:         --------   %20zu\n"
          "capacity:     --------   %20zu\n"
          "bytes:        --------   %20zu\n"
          "storage:      --------   %20zu\n",
          p_stack->minCapacity,
          p_stack->size,
          p_stack->capacity,
          p_stack->bytes,
          p_stack->storage );

#ifdef STK_CANARY_PROTECT
   printf("lCan:         --------   %20zu\n"
          "rCan:         --------   %20zu\n",
          p_stack->leftCanary,
          p_stack->rightCanary);
   
   if (p_stack->storage)
      printf("slCan:      --------   %20zu\n"
             "srCan:      --------   %20zu\n",
             *(canary_t *)find_storage_leftCanary(p_stack->storage),
             *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes));
#endif

#ifdef STK_HASH_PROTECT
   printf("hash:       ---------   %20zu\n",
          p_stack->hash);
#endif
   
   /*const char OK[] = "ok";
   const char BAD[] = "bad";
   
   const bool is_address_ok               = p_stack != nullptr,
              is_size_ok                  = p_stack->size <= p_stack->capacity,
              is_capacity_ok              = p_stack->capacity >= p_stack->size
                                            && p_stack->capacity >= p_stack->minCapacity
                                            && p_stack->capacity % 2 == 0,
              is_minCapacity_ok           = p_stack->capacity >= p_stack->minCapacity,
              is_cock_ok                  = p_stack->leftCanary == STK_CANARY,
              is_hen_ok                   = p_stack->rightCanary == STK_CANARY;
   
   const bool is_storage_ok               = p_stack->storage != nullptr,
              is_storage_size_in_bytes_ok = p_stack->storage != nullptr
                                         ? p_stack->bytes != 0 && p_stack->bytes % 8 == 0
                                         : p_stack->bytes == 0;

   size_t verifyHash = 0;
   
//   calc_stack_hash(p_stack, &verifyHash); /// add error processing

   
   const bool is_hash_ok = verifyHash == p_stack->hash;
   
   printf("Dump from: %s (%d)\n"
          "address:     %20zx %s\n"
          "size:        %20zu %s\n"
          "capacity:    %20zu %s\n"
          "minCapacity: %20zu %s\n"
          "storage:     %20zx %s\n"
          "bytes:       %20zu %s\n"
          "lCan:        %20zu %s\n"
          "rCan:        %20zu %s\n"
          "hash:        %20zu %s\n",
          file, line,
          p_stack, is_address_ok ? OK : BAD,
          p_stack->size, is_size_ok ? OK : BAD,
          p_stack->capacity, is_capacity_ok ? OK : BAD,
          p_stack->minCapacity, is_minCapacity_ok ? OK : BAD,
          p_stack->storage, is_storage_ok ? OK : BAD,
          p_stack->bytes, is_storage_size_in_bytes_ok ? OK : BAD,
          p_stack->leftCanary, is_cock_ok ? OK : BAD,
          p_stack->rightCanary, is_hen_ok ? OK : BAD,
          p_stack->hash, is_hash_ok ? OK : BAD);
   
   if (p_stack->storage)
   {
      printf("lsCan:       %20zu %s\n"
             "rsCan:       %20zu %s\n",
             *(canary_t *)find_storage_leftCanary(p_stack->storage), *(canary_t *)find_storage_leftCanary(p_stack->storage) == STK_CANARY ? OK : BAD,
             *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes), *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) == STK_CANARY ? OK : BAD);
   }*/

   if (find_firstElement(p_stack->storage, p_stack->size))
      for (element_t *p_element = (element_t *)find_firstElement(p_stack->storage, p_stack->size);
           p_element - (element_t *)find_firstElement(p_stack->storage, p_stack->size) < p_stack->capacity;
           p_element++)
         printf("|%zu:\t\t  %20" STK_SPECIFIER "   \n", p_element - (element_t *)find_firstElement(p_stack->storage, p_stack->size), *p_element);
}


static int emptyStack_inspector(const Stack *const p_stack)
{
   if (p_stack->size != 0)
   {
      return 1;
   }
   
   if (p_stack->capacity != 0)
   {
      return 1;
   }
   
   if (p_stack->minCapacity != 0)
   {
      return 1;
   }
   
   if (p_stack->bytes != 0)
   {
      return 1;
   }
   
   if (p_stack->storage != nullptr)
   {
      return 1;
   }


#ifdef STK_CANARY_PROTECT
   if (p_stack->leftCanary != 0)
   {
      return 1;
   }
   
   if (p_stack->rightCanary != 0)
   {
      return 1;
   }
#endif


#ifdef STK_HASH_PROTECT
   if (p_stack->hash != 0)
   {
      return 1;
   }
#endif
   
   return 0;
}

static size_t calc_storage_bytes(const size_t capacity)
{
   if (capacity == 0)
      return 0;

#ifdef STK_CANARY_PROTECT
   size_t       bytes           = 0;
   const size_t canary_bytes    = sizeof(canary_t);
   const size_t container_bytes = sizeof(element_t) * capacity;
   
   bytes = container_bytes / canary_bytes * canary_bytes;
   
   if (bytes != container_bytes)
      bytes += canary_bytes;
   
   bytes += canary_bytes * 2;
   
   return bytes;
#else
   return sizeof(element_t) * capacity;
#endif
}


static int calc_min_capacity(const size_t userMinCapacity, size_t *const p_minCapacity, bitmask_t *const p_bitmask)
{
   if (userMinCapacity > STK_MAX_CAPACITY)
   {
      *p_bitmask |= StackStatementDetails::WRONG_MIN_CAPACITY;
      return 1;
   }
   
   if (userMinCapacity == 0)
      *p_minCapacity = 0;
   else
   {
      size_t minCapacity = 2;
   
      while (minCapacity < userMinCapacity)
         minCapacity *= 2;
      
      *p_minCapacity = minCapacity;
   }
   
   return 0;
}


static int storage_update(const size_t capacity, const size_t size,
                          void **const p_storage, size_t *const p_bytes, bitmask_t *const p_bitmask)
{
   size_t  bytes   = 0;
   void   *storage = nullptr;
   
   if (capacity == 0)
      free(*p_storage);
   else
   {
      bytes   = calc_storage_bytes(capacity);
      storage = realloc(*p_storage, bytes);
      
      if (storage == nullptr)
      {
         *p_bitmask |= StackStatementDetails::STORAGE_NOT_UPDATED;
         return 1;
      }
   
      void       *p_storage_emptySpace  = nullptr;

#ifdef STK_CANARY_PROTECT
      void *const p_storage_leftCanary  = find_storage_leftCanary(storage);
      void *const p_storage_rightCanary = find_storage_rightCanary(storage, bytes);
      
      *(canary_t *)p_storage_leftCanary = STK_CANARY;
      *(canary_t *)p_storage_rightCanary = STK_CANARY;
      
      if (size == 0)
         p_storage_emptySpace = (canary_t *)p_storage_leftCanary + 1;
      else
         p_storage_emptySpace = (element_t *)find_lastElement(storage, size) + 1;

      memset(p_storage_emptySpace, STK_POISON, (char *)p_storage_rightCanary - (char *)p_storage_emptySpace);
      
      /// ?unstable
//      for (; (element_t *)p_storage_rightCanary - (element_t *)p_storage_emptySpace > 0;
//             p_storage_emptySpace = (element_t *)p_storage_emptySpace + 1)
//         *(element_t *)p_storage_emptySpace = STK_POISON;
#else
      if (size == 0)
         p_storage_emptySpace = (element_t *)storage;
      else
         p_storage_emptySpace = (element_t *)find_lastElement(storage, size) + 1;
      
      memset(p_storage_emptySpace, STK_POISON, bytes - ((char *)p_storage_emptySpace - (char *)storage));
#endif
   }
   
   *p_storage = storage;
   *p_bytes   = bytes;
   
   return 0;
}


static int stack_resize(Stack *const p_stack, const char operation, bitmask_t *const p_bitmask)
{
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   if (operation == '+')
   {
      if (prevSize == STK_MAX_CAPACITY)
      {
         *p_bitmask |= StackDetails::FULL;
         return 1;
      }
      
      newSize = prevSize + 1;

      if (prevCapacity == 0)
         newCapacity = 2;
      else if (newSize <= prevCapacity)
         newCapacity = prevCapacity;
      else
         newCapacity = prevCapacity * 2;
   }
   else if (operation == '-')
   {
      if (prevSize == 0)
      {
         *p_bitmask |= StackDetails::EMPTY;
         return 1;
      }
      
      newSize = prevSize - 1;

      if (newSize <= minCapacity)
         newCapacity = minCapacity;
      else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
         newCapacity = prevCapacity / 2; //! opt
      else
         newCapacity = prevCapacity;
   }
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
      
   return storage_update(p_stack->capacity, p_stack->size, &p_stack->storage, &p_stack->bytes, p_bitmask);
}


static int stack_inspector(const Stack *const p_stack, bitmask_t *const p_bitmask) {

#ifdef STK_CANARY_PROTECT
   if (p_stack->leftCanary != STK_CANARY)
   {
      *p_bitmask |= StackDetails::BANNED;
   }
   
   if (p_stack->rightCanary != STK_CANARY)
   {
      *p_bitmask |= StackDetails::BANNED;
   }
   
   if (p_stack->storage != nullptr)
   {
      if (*(canary_t *)find_storage_leftCanary(p_stack->storage) != STK_CANARY)
      {
         *p_bitmask |= StackDetails::STORAGE_LEFT_CANARY_ATTACKED;
         *p_bitmask |= StackDetails::BANNED;
      }
      
      if (*(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) != STK_CANARY)
      {
         *p_bitmask |= StackDetails::STORAGE_RIGHT_CANARY_ATTACKED;
         *p_bitmask |= StackDetails::BANNED;
      }
   }
#endif

#ifdef STK_HASH_PROTECT
   size_t verifyHash = 0;
   
   /// ?change expression
   if (calc_stack_hash(p_stack, &verifyHash))
   {
      *p_bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
   }
   
   if (p_stack->hash != verifyHash)
   {
      *p_bitmask |= StackStatementDetails::HASH_NOT_VERIFIED | StackDetails::BANNED;
   }
#endif
   
   if (*p_bitmask != 0)
      return 1;
   
   return 0;
}


bitmask_t stack_init(Stack *const p_stack, const size_t userMinCapacity)
{
   bitmask_t bitmask = 0;
   
   if (p_stack == nullptr) {
      return bitmask | StackStatementDetails::STACK_NULLPTR;
   }

#ifndef STK_UNPROTECT
   if (emptyStack_inspector(p_stack)) {
      return bitmask | StackStatementDetails::REINITIALIZATION;
   }
#endif
   
   size_t minCapacity = 0;
   
   if (calc_min_capacity(userMinCapacity, &minCapacity, &bitmask))
      return bitmask;
   
   p_stack->minCapacity = minCapacity;
   p_stack->capacity    = minCapacity;
   p_stack->size        = 0;
   p_stack->bytes       = 0;
   p_stack->storage     = nullptr;
   
#ifdef STK_CANARY_PROTECT
   p_stack->leftCanary  = STK_CANARY;
   p_stack->rightCanary = STK_CANARY;
#endif
   
   storage_update(p_stack->capacity, p_stack->size, &p_stack->storage, &p_stack->bytes, &bitmask);

#ifdef STK_HASH_PROTECT
   p_stack->hash = 0;
   
   if (calc_stack_hash(p_stack, &p_stack->hash))
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
#endif
   
   return bitmask;
}


bitmask_t stack_push(Stack *const p_stack, const element_t element)
{
   bitmask_t bitmask = 0;

   if (p_stack == nullptr)
      return bitmask | StackStatementDetails::STACK_NULLPTR;

#ifndef STK_UNPROTECT
   if (stack_inspector(p_stack, &bitmask))
      return bitmask;
#endif
   
   if (stack_resize(p_stack, '+', &bitmask) == 0)
      *(element_t *)find_lastElement(p_stack->storage, p_stack->size) = element;

#ifdef STK_HASH_PROTECT
   if (calc_stack_hash(p_stack, &p_stack->hash))
   {
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
      bitmask |= StackDetails::BANNED;
   }
#endif

   return bitmask;
}


bitmask_t stack_pop(Stack *const p_stack, element_t *const p_output)
{
   bitmask_t bitmask = 0;
   
   if (p_stack == nullptr)
      bitmask |= StackStatementDetails::STACK_NULLPTR;
   
   if (p_output == nullptr)
      bitmask |= StackStatementDetails::OUTPUT_NULLPTR;
   
   if (bitmask != 0)
      return bitmask;
   
#ifndef STK_UNPROTECT
   if (stack_inspector(p_stack, &bitmask))
      return bitmask;
#endif
   
   *p_output = *(element_t *)find_lastElement(p_stack->storage, p_stack->size);
   
   stack_resize(p_stack, '-', &bitmask);

#ifdef STK_HASH_PROTECT
   if (calc_stack_hash(p_stack, &p_stack->hash))
   {
      bitmask |= StackStatementDetails::MEMORY_NOT_ALLOCATED;
      bitmask |= StackDetails::BANNED;
   }
#endif

   return bitmask;
}

void stack_destroy(Stack *const p_stack)
{
   if (p_stack != nullptr)
   {
      free(p_stack->storage);
   
      p_stack->size          = 0;
      p_stack->capacity      = 0;
      p_stack->minCapacity   = 0;
      p_stack->storage       = nullptr;
      p_stack->bytes         = 0;

#ifdef STK_CANARY_PROTECT
      p_stack->leftCanary    = 0;
      p_stack->rightCanary   = 0;
#endif

#ifdef STK_HASH_PROTECT
      p_stack->hash          = 0;
#endif
   }
}

