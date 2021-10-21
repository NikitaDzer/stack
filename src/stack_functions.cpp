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
   return (element_t *)((char *)p_stack->storage
                        + sizeof(STK_COCK_HEN)
                        + sizeof(element_t) * (p_stack->size - 1)); //? correctly
}

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
   
   
   for (element_t *p_element = find_firstElement(p_stack);
        p_element <= find_lastElement(p_stack);
        p_element++)
      printf("|%zu:\t\t  %20" STK_SPECIFIER "   \n", p_element - find_firstElement(p_stack), *p_element);
}

//! try other definition
static bool is_stack_inited(Stack* p_stack)
{
   return    p_stack->cock          == STK_COCK_HEN
          || p_stack->size          != 0
          || p_stack->capacity      != 0
          || p_stack->minCapacity   != 0
          || p_stack->storage       != nullptr
          || p_stack->storage_bytes != 0
          || p_stack->hen           == STK_COCK_HEN;
}

static bool is_stack_invalid(const Stack *const p_stack)
{
   if (p_stack->storage == nullptr && p_stack->size == 0)
      return false;

   if (p_stack->size <= p_stack->capacity)
      return false;

   if (p_stack->minCapacity <= p_stack->capacity)
      return false;
   
   return true;
}

static size_t calc_storage_bytes(const Stack *const p_stack)
{
   const size_t capacity              = p_stack->capacity;
   size_t       storage_size_in_bytes = 0;
   
   if (capacity != 0)
   {
      const size_t cock_size_in_bytes      = sizeof(STK_COCK_HEN),
              container_size_in_bytes = sizeof(element_t) * capacity;
      
      storage_size_in_bytes = container_size_in_bytes / cock_size_in_bytes * cock_size_in_bytes;
      
      if (storage_size_in_bytes != container_size_in_bytes)
         storage_size_in_bytes += cock_size_in_bytes;
      
      storage_size_in_bytes += cock_size_in_bytes * 2;
   }
   
   return storage_size_in_bytes;
}

static int stack_realloc(Stack *const p_stack) //! make poison filler
{
   element_t *storage       = nullptr;
   size_t     storage_bytes = 0;
   
   if (p_stack->capacity == 0)
      free(p_stack->storage); //! opt
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

static int stack_resize(Stack *const p_stack, const char operation)
{
   //! align declarations
   const size_t minCapacity  = p_stack->minCapacity;
   const size_t prevSize     = p_stack->size;
   const size_t prevCapacity = p_stack->capacity;
   size_t       newSize      = 0;
   size_t       newCapacity  = 0;
   
   
   if (operation == '+')
   {
      if (prevSize == STK_MAX_CAPACITY)
         return 1;
      
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
         return 1;
      
      newSize = prevSize - 1;
      
      if (newSize <= minCapacity)
         newCapacity = minCapacity;
      else if (newSize * 2 <= prevCapacity) //! opt
         newCapacity = prevCapacity / 2; //! opt
      else
         newCapacity = prevCapacity;
   }
   
   p_stack->size     = newSize;
   p_stack->capacity = newCapacity;
   
   stack_realloc(p_stack);
   
   return 0;
}

static StackError stack_inspector(const Stack *const p_stack) {
   
   if (p_stack == nullptr)
      return StackError::STACK_NULLPTR;
   
   if ( p_stack->size > p_stack->capacity ||
        p_stack->minCapacity > p_stack->capacity ||
        (p_stack->storage == nullptr && p_stack->size != 0 && p_stack->capacity != 0) ||
        p_stack->cock != STK_COCK_HEN ||
        p_stack->hen != STK_COCK_HEN)
      return StackError::STACK_BANNED;

//   if (p_stack->elements == nullptr && p_stack->size == 0) {
//      return STK_ERROR__STACK_NULLPTR;
//   }
//
//
//   if (p_stack->minCapacity <= p_stack->capacity)
//      return STK_ERROR__STACK_NULLPTR;

}

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






StackError stack_init(Stack *const p_stack, const size_t desiredMinCapacity)
{
   if (p_stack == nullptr)
      return StackError::STACK_NULLPTR;
   
   if (is_stack_inited(p_stack))
      return StackError::STACK_REINITIALIZATION;
   
   size_t minCapacity = calc_min_capacity(desiredMinCapacity);
   
   p_stack->cock          = STK_COCK_HEN;
   p_stack->size          = 0;
   p_stack->capacity      = minCapacity;
   p_stack->minCapacity   = minCapacity;
   p_stack->storage       = nullptr;
   p_stack->storage_bytes = 0;
   p_stack->hen           = STK_COCK_HEN;
   
   if (stack_realloc(p_stack))
      return StackError::UNALLOCATED_MEMORY;
   
   return StackError::NOERR;
}

StackError stack_push(Stack *const p_stack, const element_t element)
{
   if (p_stack == nullptr)
      return StackError::STACK_NULLPTR;

   if (is_stack_invalid(p_stack))
      return StackError::STACK_MISUSE;
      
   if (stack_resize(p_stack, '+'))
      return StackError::STACK_EXCEEDED_MAX_CAPACITY;
   
   *find_lastElement(p_stack) = element;
   
   return StackError::NOERR;
}

StackError stack_pop(Stack *const p_stack, element_t *const p_output)
{
   if (p_stack == nullptr)
      return StackError::STACK_NULLPTR;
   
   if (is_stack_invalid(p_stack))
      return StackError::STACK_MISUSE;
   
   if (p_stack->size == 0)
      return StackError::STACK_EMPTY;
   
   if (p_output == nullptr)
     return StackError::OUTPUT_NULLPTR;
   
   *p_output = *find_lastElement(p_stack);
   
   if (stack_resize(p_stack, '-'))
      return StackError::STACK_EXCEEDED_MAX_CAPACITY;
   
   if (stack_realloc(p_stack))
      return StackError::STACK_NON_UPDATED_ELEMENTS;

   return StackError::NOERR;
}

StackError stack_destroy(Stack *const p_stack)
{
   if (p_stack == nullptr)
      return StackError::STACK_NULLPTR;
   
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
