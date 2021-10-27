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

static inline void* find_storage_leftCanary(const void *const storage)
{
   return (void *)storage;
}

static inline void* find_storage_rightCanary(const void *const storage, const size_t bytes)
{
   if (storage == nullptr)
      return nullptr;
   
   return (void *)((char *)storage + bytes - sizeof(canary_t));
}
#endif


static inline void* find_firstElement(const void *const storage, const size_t size)
{
   if (storage == nullptr || size == 0)
      return nullptr;

#ifdef STK_CANARY_PROTECT
   return (void *)((char *)storage + sizeof(canary_t));
#else
   return (void *)storage;
#endif
}

static inline void* find_lastElement(const void *const storage, const size_t size)
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
static int calc_stack_hash(const Stack *const p_stack, hash_t *const p_hash)
{
   const size_t        bytes = p_stack->bytes;
   void         *const data  = malloc(sizeof(Stack) + bytes);
   
   if (data == nullptr) {
      return 1;
   }
   
   memcpy(data,                         p_stack,          sizeof(Stack));
   memcpy((char *)data + sizeof(Stack), p_stack->storage, bytes);
   memset((char *)data + ((char *)&p_stack->hash - (char *)p_stack), 0x00, sizeof(hash_t));
   
   *p_hash = quickHash(data, sizeof(Stack) + bytes);
   
   free(data);
   
   return 0;
}
#endif

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
   
   if (p_stack->storage != nullptr)
   {
      return 1;
   }


#ifdef STK_CANARY_PROTECT
   if (p_stack->bytes != 0)
   {
      return 1;
   }
   
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
   if (userMinCapacity > STK_MAX_CAPACITY_)
   {
      *p_bitmask |= StackStatementDetails::WRONG_MIN_CAPACITY;
      return 1;
   }
   
   if (userMinCapacity == 0)
      *p_minCapacity = 0;
   else
   {
      size_t minCapacity = 0;
   
      if (userMinCapacity / 2 * 2 == userMinCapacity)
         minCapacity = userMinCapacity;
      else
         minCapacity = (userMinCapacity / 2 + 1) * 2;
      
      *p_minCapacity = minCapacity;
   }
   
   return 0;
}

#ifdef STK_CANARY_PROTECT
static int storage_update(void **const p_storage, size_t *const p_bytes,
                          const size_t capacity, const size_t size, bitmask_t *const p_bitmask)
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
      void *const p_storage_leftCanary  = find_storage_leftCanary(storage);
      void *const p_storage_rightCanary = find_storage_rightCanary(storage, bytes);
      
      *(canary_t *)p_storage_leftCanary = STK_CANARY;
      *(canary_t *)p_storage_rightCanary = STK_CANARY;
      
      if (size == 0)
         p_storage_emptySpace = (canary_t *)p_storage_leftCanary + 1;
      else
         p_storage_emptySpace = (element_t *)find_lastElement(storage, size) + 1;

      memset(p_storage_emptySpace, STK_POISON, (char *)p_storage_rightCanary - (char *)p_storage_emptySpace);
   }
   
   *p_storage = storage;
   *p_bytes   = bytes;
   
   return 0;
}
#else
static int storage_update(void **const p_storage, const size_t capacity, const size_t size, bitmask_t *const p_bitmask)
{
   void   *storage = nullptr;
   
   if (capacity == 0)
      free(*p_storage);
   else
   {
      const size_t bytes = calc_storage_bytes(capacity);
      
      storage = realloc(*p_storage, bytes);
      
      if (storage == nullptr)
      {
         *p_bitmask |= StackStatementDetails::STORAGE_NOT_UPDATED;
         return 1;
      }
      
      void *p_storage_emptySpace  = nullptr;
      
      if (size == 0)
         p_storage_emptySpace = (element_t *)storage;
      else
         p_storage_emptySpace = (element_t *)find_lastElement(storage, size) + 1;
      
      memset(p_storage_emptySpace, STK_POISON, bytes - ((char *)p_storage_emptySpace - (char *)storage));
   }
   
   *p_storage = storage;
   
   return 0;
}
#endif

static int stack_resize(Stack *const p_stack, const char operation, bitmask_t *const p_bitmask)
{
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   if (operation == '+')
   {
      if (prevSize == STK_MAX_CAPACITY_)
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
   
#ifdef STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif
}

static int stack_size_increase(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (p_stack->size == STK_MAX_CAPACITY_)
   {
      *p_bitmask |= StackDetails::FULL;
      return 1;
   }
   
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size + 1;
   size_t       newCapacity  = 0;
   
   if (prevCapacity == 0)
      newCapacity = 2;
   else if (newSize <= prevCapacity)
      newCapacity = prevCapacity;
   else
      newCapacity = prevCapacity * 2;
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;

#ifdef STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif
}

static int stack_size_decrease(Stack *const p_stack, bitmask_t *const p_bitmask)
{
   if (p_stack->size == 0)
   {
      *p_bitmask |= StackDetails::EMPTY;
      return 1;
   }
   
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevCapacity = p_stack->capacity;
   const size_t newSize      = p_stack->size - 1;
   size_t       newCapacity  = 0;
   
   if (newSize <= minCapacity)
      newCapacity = minCapacity;
   else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
      newCapacity = prevCapacity / 2; //! opt
   else
      newCapacity = prevCapacity;
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;

#ifdef STK_CANARY_PROTECT
   return storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size,  p_bitmask);
#else
   return storage_update(&p_stack->storage, p_stack->capacity, p_stack->size,  p_bitmask);
#endif
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

#ifndef STK_PRODUCTION
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
   p_stack->storage     = nullptr;

#ifdef STK_CANARY_PROTECT
   p_stack->leftCanary  = STK_CANARY;
   p_stack->bytes       = 0;
   p_stack->rightCanary = STK_CANARY;
   
   storage_update(&p_stack->storage, &p_stack->bytes, p_stack->capacity, p_stack->size, &bitmask);
#else
   storage_update(&p_stack->storage, p_stack->capacity, p_stack->size, &bitmask);
#endif


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
   
   if (stack_size_increase(p_stack, &bitmask) == 0)
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
   
   stack_size_decrease(p_stack, &bitmask);

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
      
#ifdef STK_CANARY_PROTECT
      p_stack->leftCanary    = 0;
      p_stack->bytes         = 0;
      p_stack->rightCanary   = 0;
#endif

#ifdef STK_HASH_PROTECT
      p_stack->hash          = 0;
#endif
   }
}
#endif


