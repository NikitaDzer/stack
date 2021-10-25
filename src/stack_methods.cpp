#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../include/stack_functions.h"

/*!
 * @brief Typedef for stack element's data type
 */
typedef stk_element_t element_t;


static size_t* find_storage_cock(const Stack *const p_stack)
{
   return (size_t *)p_stack->storage;
}

static size_t* find_storage_hen(const Stack *const p_stack)
{
   return (size_t *)((char *)p_stack->storage + p_stack->storage_bytes - sizeof(STK_COCK_HEN));
}

static element_t* find_firstElement(const Stack *const p_stack)
{
   return (element_t *)((char *)p_stack->storage + sizeof(STK_COCK_HEN));
}

static element_t* find_lastElement(const Stack *const p_stack)
{
   if (p_stack->storage == nullptr)
      return nullptr;
   
   return (element_t *)((char *)p_stack->storage
                        + sizeof(STK_COCK_HEN)
                        + sizeof(element_t) * (p_stack->size - 1));
}

/// status: shit
/// fix:    everything
/// make:   #define instead of function
void stack_dump( const Stack *const p_stack, const char *const file, const int line )
{
   
   const char OK[] = "ok";
   const char BAD[] = "bad";
   
   bool is_address_ok               = p_stack != nullptr,
           is_size_ok                  = p_stack->size <= p_stack->capacity,
           is_capacity_ok              = p_stack->capacity >= p_stack->size
                                         && p_stack->capacity >= p_stack->minCapacity
                                         && p_stack->capacity % 2 == 0,
           is_minCapacity_ok           = p_stack->capacity >= p_stack->minCapacity,
           is_storage_ok               = p_stack->storage != nullptr,
           is_storage_size_in_bytes_ok = p_stack->storage != nullptr
                                         ? p_stack->storage_bytes != 0 && p_stack->storage_bytes % 8 == 0
                                         : p_stack->storage_bytes == 0,
           is_cock_ok                  = p_stack->cock == STK_COCK_HEN,
           is_hen_ok                   = p_stack->hen == STK_COCK_HEN,
           is_s_cock_ok                = *find_storage_cock(p_stack) == STK_COCK_HEN,
           is_s_hen_ok                 = *find_storage_hen(p_stack) == STK_COCK_HEN;
   
   printf("Dump from: %s (%d)\n"
          "address:     %20zx %s\n"
          "size:        %20zu %s\n"
          "capacity:    %20zu %s\n"
          "minCapacity: %20zu %s\n"
          "storage:     %20zx %s\n"
          "ssib:        %20zu %s\n"
          "cock:        %20zu %s\n"
          "hen:         %20zu %s\n"
          "s_cock:      %20zu %s\n"
          "s_hen:       %20zu %s\n"
          "elements:\n",
          file, line,
          p_stack, is_address_ok ? OK : BAD,
          p_stack->size, is_size_ok ? OK : BAD,
          p_stack->capacity, is_capacity_ok ? OK : BAD,
          p_stack->minCapacity, is_minCapacity_ok ? OK : BAD,
          p_stack->storage, is_storage_ok ? OK : BAD,
          p_stack->storage_bytes, is_storage_size_in_bytes_ok ? OK : BAD,
          p_stack->cock, is_cock_ok ? OK : BAD,
          p_stack->hen, is_hen_ok ? OK : BAD,
          *find_storage_cock(p_stack), is_s_cock_ok ? OK : BAD,
          *find_storage_hen(p_stack), is_s_hen_ok ? OK : BAD);
   
   
//   for (element_t *p_element = find_firstElement(p_stack);
//        p_element <= find_lastElement(p_stack);
//        p_element++)
//      printf("|%zu:\t\t  %20" STK_SPECIFIER "   \n", p_element - find_firstElement(p_stack), *p_element);
}

/// status: stable
/// fix:    -
/// make:   -
static bool is_stack_inited(Stack* p_stack)
{
   return    p_stack->cock == STK_COCK_HEN
          && p_stack->hen  == STK_COCK_HEN;
}

/// status: stable
/// fix:    ?optimization
/// make:   -
static size_t calc_storage_bytes(const Stack *const p_stack)
{
   const size_t capacity      = p_stack->capacity;
   size_t       storage_bytes = 0;
   
   if (capacity != 0)
   {
      const size_t cock_bytes      = sizeof(STK_COCK_HEN),
                   container_bytes = sizeof(element_t) * capacity;
   
      storage_bytes = container_bytes / cock_bytes * cock_bytes;
      
      if (storage_bytes != container_bytes)
         storage_bytes += cock_bytes;
   
      storage_bytes += cock_bytes * 2;
   }
   
   return storage_bytes;
}

/// status: stable
/// fix:    name and parameters
/// make:   poison filler
static int stack_storage_update(Stack *const p_stack)
{
   element_t *storage       = nullptr;
   size_t     storage_bytes = 0;
   
   if (p_stack->capacity == 0)
      free(p_stack->storage);
   else
   {
      storage_bytes = calc_storage_bytes(p_stack);
      storage       = (element_t *)realloc((void *)p_stack->storage, storage_bytes);
      
      if (storage == nullptr)
         return 1;
   
      *(size_t *)storage = STK_COCK_HEN;
      *(size_t *)((char *)storage + storage_bytes - sizeof(STK_COCK_HEN)) = STK_COCK_HEN;
   }
   
   p_stack->storage       = storage;
   p_stack->storage_bytes = storage_bytes;
   
   return 0;
}

/// status: non ready
/// fix:    ?optimization, ?decreaser
/// make:   -
static StackError stack_resize(Stack *const p_stack, const char operation)
{
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   if (operation == '+')
   {
      if (prevSize == STK_MAX_CAPACITY)
         return StackError::MAX_SIZE;
      
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
         return StackError::EMPTY;
      
      newSize = prevSize - 1;
      
      if (newSize <= minCapacity)
         newCapacity = minCapacity;
      else if (newSize * 2 <= prevCapacity && prevCapacity >= 4) //! opt
         newCapacity = prevCapacity / 2; //! opt
      else
         newCapacity = prevCapacity;
   
      printf("%zu ", newSize);
   }
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
   if (stack_storage_update(p_stack))
      return StackError::NON_UPDATED;
   
   return StackError::NOERR;
}

static StackError stack_inspector(const Stack *const p_stack) {
   
   if (p_stack == nullptr)
      return StackError::NULLPTR;
   
   if ( p_stack->size > p_stack->capacity ||
        p_stack->minCapacity > p_stack->capacity ||
        (p_stack->storage == nullptr && p_stack->size != 0 && p_stack->capacity != 0) ||
        p_stack->cock != STK_COCK_HEN ||
        p_stack->hen != STK_COCK_HEN)
      return StackError::BANNED;

//   if (p_stack->elements == nullptr && p_stack->size == 0) {
//      return STK_ERROR__STACK_NULLPTR;
//   }
//
//
//   if (p_stack->minCapacity <= p_stack->capacity)
//      return STK_ERROR__STACK_NULLPTR;

}

/// status: stable
/// fix:    ?optimization
/// make: -
static size_t calc_min_capacity(const size_t desiredMinCapacity)
{
   if (desiredMinCapacity == 0)
      return 0;
   
   if (desiredMinCapacity > STK_MAX_CAPACITY)
      return STK_MAX_CAPACITY;
   
   size_t minCapacity = 2;
   
   while (minCapacity < desiredMinCapacity)
      minCapacity *= 2;
   
   return minCapacity;
}

/// status: non tested
/// fix:    ?reinitialization
/// make:   -
StackError stack_init(Stack *const p_stack, const size_t desiredMinCapacity)
{
   if (p_stack == nullptr)
      return StackError::NULLPTR;
   
   if (is_stack_inited(p_stack))
      return StackError::REINITIALIZATION;
   
   size_t minCapacity = calc_min_capacity(desiredMinCapacity);
   
   p_stack->cock          = STK_COCK_HEN;
   p_stack->size          = 0;
   p_stack->capacity      = minCapacity;
   p_stack->minCapacity   = minCapacity;
   p_stack->storage       = nullptr;
   p_stack->storage_bytes = 0;
   p_stack->hen           = STK_COCK_HEN;
   
   if (stack_storage_update(p_stack))
      return StackError::NON_ALLOCATED;
   
   return StackError::NOERR;
}


/// status: stable
/// fix:    -
/// make:   error processing
StackError stack_push(Stack *const p_stack, const element_t element)
{
   if (p_stack == nullptr)
      return StackError::NULLPTR;

   if (stack_resize(p_stack, '+'))
      return StackError::MAX_SIZE;
   
   *find_lastElement(p_stack) = element;
   
   return StackError::NOERR;
}


/// status: unstable
/// fix:    logic of size decreasing, size decreasing
/// make:   error processing
StackError stack_pop(Stack *const p_stack, element_t *const p_output)
{
   if (p_stack == nullptr)
      return StackError::NULLPTR;
   
   if (p_output == nullptr)
      return StackError::OUTPUT_NULLPTR;
   
   if (p_stack->size == 0)
      return StackError::EMPTY;
   
   *p_output = *find_lastElement(p_stack);

   stack_resize(p_stack, '-');
   
   return StackError::NOERR;
}


/// status: stable
/// fix:    -
/// make:   -
StackError stack_destroy(Stack *const p_stack)
{
   if (p_stack == nullptr)
      return StackError::NULLPTR;
   
   free(p_stack->storage);
   
   p_stack->cock          = 0;
   p_stack->size          = 0;
   p_stack->capacity      = 0;
   p_stack->minCapacity   = 0;
   p_stack->storage       = nullptr;
   p_stack->storage_bytes = 0;
   p_stack->hen           = 0;
   
   return StackError::NOERR;
}
