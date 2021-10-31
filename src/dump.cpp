

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include "../include/dump.h"
#include "../prototyping/prototyping.h"

#ifndef STK_PRODUCTION
#ifdef  STK_DEBUG
typedef stk_element_t element_t;

typedef stk_bitmask_t bitmask_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
#endif

struct Dump
{
   const Stack *p_stack;
   char*  log;
   size_t size;
   long   init_line;
   long   ban_line;
   StackLogCallers prevCaller;
};

static const size_t MAX_FORMATTED_STRING_SIZE = 1024 - 1;
static size_t dumps_counter = 0;
static Dump  *dumps         = nullptr;
static FILE  *logfile       = fopen(STK_LOGFILE_PATH, "w");
static char  templog[MAX_FORMATTED_STRING_SIZE + 1] = "";

#ifdef STK_CANARY_PROTECT
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
#ifdef STK_CANARY_PROTECT
   const size_t        bytes = p_stack->bytes;
#else
   const size_t        bytes = sizeof(element_t) * p_stack->capacity;
#endif
   void         *const data  = malloc(sizeof(Stack) + bytes);
   
   if (data == nullptr) {
      return 1;
   }
   
   memcpy(data,                         p_stack,          sizeof(Stack));
   memcpy((char *)data + sizeof(Stack), p_stack->storage, bytes);
   memset((char *)data + ((char *)&p_stack->hash - (char *)p_stack), 0x00, sizeof(hash_t));
   
   *p_hash = qhash(data, sizeof(Stack) + bytes);
   
   free(data);
   
   return 0;
}
#endif

static int dump_update(Dump *const p_dump, const char *const format, ...)
{
   va_list ap = {};
   va_start(ap, format);
   
   p_dump->size += vsprintf(templog, format, ap) + 1;
   p_dump->log   = (char *)realloc(p_dump->log, p_dump->size * sizeof(char));
   
   strcat(p_dump->log, templog);
   
   va_end(ap);
   
   return 0;
}

static Dump* find_dump(const Stack *const p_stack)
{
   for (size_t i = 0; i < dumps_counter; i++)
      if (dumps[i].p_stack == p_stack)
         return (Dump *)dumps + i;
   
   return nullptr;
}

static Dump* create_dump(const Stack *const p_stack)
{
   dumps_counter += 1;
   dumps = (Dump *)realloc(dumps, sizeof(Dump) * dumps_counter);
   
   Dump* const p_dump = (Dump *)dumps + dumps_counter - 1;
   
   p_dump->p_stack    =  p_stack;
   p_dump->log        =  nullptr;
   p_dump->size       =  0;
   p_dump->init_line  = -1;
   p_dump->ban_line   = -1;
   p_dump->prevCaller = CALLER_NOONE;
   
   dump_update(p_dump, "\n"
                       "================================\n"
                       "Stack (%zu), address (%p)\n",
                       dumps_counter, p_stack);
   
   return p_dump;
}

static Dump* get_dump(const Stack *const p_stack)
{
   Dump* p_dump = find_dump(p_stack);
   
   if (p_dump == nullptr)
      p_dump = create_dump(p_stack);
   
   return p_dump;
}

static int process_init(Dump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
{
   if (p_dump->ban_line == -1)
   {
      if (p_dump->prevCaller != CALLER_STACK_INIT)
      {
         dump_update(p_dump, "------------------------------\n");
      }
      else
      {
         dump_update(p_dump, "\n");
      }
      
      if (bitmask == 0)
      {
         dump_update(p_dump, "Init success (%s, %ld)\n", filename, line);
   
         if (p_dump->init_line >= 0)
         {
            dump_update(p_dump, "WARNING! This stack has already been inited at line (%ld) and not destroyed,\n"
                                "         but stack_init won't signal about it.\n"
                                "         May be data loss\n",
                                p_dump->init_line);
         }
         else
         {
            p_dump->init_line = line;
         }
      }
      else
      {
         dump_update(p_dump, "Init fail (%s, %ld)\n", filename, line);
         
         if (bitmask & StackStatementDetails::CANDIDATE_NOT_CLEAR)
         {
            dump_update(p_dump, "- candidate stack isn't clean\n");
         }
         
         if (bitmask & StackStatementDetails::REINITIALIZATION)
         {
            dump_update(p_dump, "- stack has already been inited at line (%ld)\n", p_dump->init_line);
         }
         
         if (bitmask & StackStatementDetails::WRONG_MIN_CAPACITY)
         {
            dump_update(p_dump, "- wrong min capacity (%zu).\n"
                                "  It must be no more, than STK_MAX_CAPACITY_ (%zu)\n",
                                va_arg(ap, size_t), STK_MAX_CAPACITY_);
         }
         
         if (bitmask & StackStatementDetails::STORAGE_NOT_UPDATED)
         {
            dump_update(p_dump, "- storage wasn't allocated.\n");
         }
         
         if (bitmask & StackStatementDetails::MEMORY_NOT_ALLOCATED)
         {
            dump_update(p_dump, "- memory for hash calculating wasn't allocated\n");
         }
      }
      
      p_dump->prevCaller = CALLER_STACK_INIT;
   }
   
   return 0;
}

static int process_push(Dump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
{
   if (p_dump->ban_line == -1)
   {
      if (p_dump->prevCaller != CALLER_STACK_PUSH)
      {
         dump_update(p_dump,
                     "------------------------------\n");
      }
      else
      {
         dump_update(p_dump, "\n");
      }
      
      if (bitmask == 0)
      {
         dump_update(p_dump,
                     "Push (%" SPECIFIER ") success (%s, %ld)\n",
                     va_arg(ap, element_t), filename, line);
   
         if (p_dump->init_line == -1)
         {
            dump_update(p_dump,
                        "WARNING! This stack hasn't been inited yet,\n"
                        "         but stack_push won't signal about it.\n"
                        "         Stack may work incorrectly\n");
         }
      }
      else
      {
         dump_update(p_dump,
                     "Push (%" SPECIFIER ") fail (%s, %ld)\n",
                     va_arg(ap, element_t), filename, line);
         
         if (bitmask & StackStatementDetails::STATEMENT_WITH_BANNED_STACK)
         {
            if (p_dump->init_line >= 0)
            {
               dump_update(p_dump,
                           "- stack has been broken by changing it's fields\n");
            }
            else
            {
               dump_update(p_dump,
                           "- stack hasn't been inited yet\n");
            }
         }
         
         if (bitmask & StackDetails::LEFT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- stack left canary attacked.\n");
         }
   
         if (bitmask & StackDetails::RIGHT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- stack right canary attacked.\n");
         }
         
         if (bitmask & StackDetails::STORAGE_LEFT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- storage left canary attacked.\n");
         }
         
         if (bitmask & StackDetails::FULL)
         {
            dump_update(p_dump,
                        "- storage full with size (%%zu)\n",
                        p_dump->p_stack->size);
         }
         
         
         if (bitmask & StackDetails::STORAGE_RIGHT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- storage right canary attacked.\n");
         }
         
         if (bitmask & StackStatementDetails::MEMORY_NOT_ALLOCATED)
         {
            dump_update(p_dump,
                        "- memory for hash calculating wasn't allocated\n");
         }
         
         if (bitmask & StackStatementDetails::HASH_NOT_VERIFIED)
         {
            hash_t verifyHash = 0;
            calc_stack_hash(p_dump->p_stack, &verifyHash);
            
            dump_update(p_dump,
                        "- hash wasn't verified\n"
                        "  stack  hash: (%zu)\n"
                        "  verify hash: (%zu)\n",
                        p_dump->p_stack->hash,
                        verifyHash);
         }
         
         if (bitmask & StackStatementDetails::STORAGE_NOT_UPDATED)
         {
            dump_update(p_dump,
                        "- storage wasn't updated\n");
         }
         
      }
      
      p_dump->prevCaller = CALLER_STACK_PUSH;
   }
   
   return 0;
}

static int process_pop(Dump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
{
   if (p_dump->ban_line == -1)
   {
      if (p_dump->prevCaller != CALLER_STACK_POP)
      {
         dump_update(p_dump,
                     "------------------------------\n");
      }
      else
      {
         dump_update(p_dump, "\n");
      }
   
      const element_t *p_output = va_arg(ap, element_t *);
      
      if (bitmask == 0)
      {
         dump_update(p_dump,
                     "Pop (%" SPECIFIER ") to (%s) with address (%p) success (%s, %ld)\n",
                      *p_output, va_arg(ap, char *), p_output, filename, line);
   
         if (p_dump->init_line == -1)
         {
            dump_update(p_dump,
                        "WARNING! This stack hasn't been inited yet,\n"
                        "         but stack_pop won't signal about it.\n"
                        "         Stack may work incorrectly\n");
         }
      }
      else
      {
         if (bitmask & StackDetails::EMPTY)
         {
            dump_update(p_dump,
                        "Pop nothing to (%s) with address (%p) fail (%s, %ld)\n"
                        "- stack is empty\n",
                        va_arg(ap, char *), p_output, filename, line);
         }
         else
         {
            dump_update(p_dump,
                        "Pop (%" SPECIFIER ") to (%s) with address (%p) fail (%s, %ld)\n",
                        *p_output, va_arg(ap, char *), p_output, filename, line);
   
         }
   
         if (bitmask & StackStatementDetails::OUTPUT_NULLPTR)
         {
            dump_update(p_dump,
                        "- output is null pointer\n");
         }
   
         if (bitmask & StackStatementDetails::STATEMENT_WITH_BANNED_STACK)
         {
            if (p_dump->init_line >= 0)
            {
               dump_update(p_dump,
                           "- stack has been broken by changing it's fields\n");
            }
            else
            {
               dump_update(p_dump,
                           "- stack hasn't been inited yet\n");
            }
         }
   
         if (bitmask & StackDetails::LEFT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- stack left canary attacked.\n");
         }
   
         if (bitmask & StackDetails::RIGHT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- stack right canary attacked.\n");
         }
   
         if (bitmask & StackDetails::STORAGE_LEFT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- storage left canary attacked.\n");
         }
   
         if (bitmask & StackDetails::STORAGE_RIGHT_CANARY_ATTACKED)
         {
            dump_update(p_dump,
                        "- storage right canary attacked.\n");
         }
   
         if (bitmask & StackStatementDetails::MEMORY_NOT_ALLOCATED)
         {
            dump_update(p_dump,
                        "- memory for hash calculating wasn't allocated\n");
         }
   
         if (bitmask & StackStatementDetails::HASH_NOT_VERIFIED)
         {
            hash_t verifyHash = 0;
            calc_stack_hash(p_dump->p_stack, &verifyHash);
      
            dump_update(p_dump,
                        "- hash wasn't verified\n"
                        "  stack  hash: (%zu)\n"
                        "  verify hash: (%zu)\n",
                        p_dump->p_stack->hash,
                        verifyHash);
         }
   
         if (bitmask & StackStatementDetails::STORAGE_NOT_UPDATED)
         {
            dump_update(p_dump,
                        "- storage wasn't updated\n");
         }
   
      }
   
      p_dump->prevCaller = CALLER_STACK_POP;
   }
   
   return 0;
}

static int process_destroy(Dump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
{
   if (p_dump->prevCaller != CALLER_STACK_DESTROY)
   {
      dump_update(p_dump,
                  "------------------------------\n");
   }
   else
   {
      dump_update(p_dump, "\n");
   }
   
   if (p_dump->init_line >= 0)
   {
      dump_update(p_dump,
                  "Destroy success (%s, %ld)\n",
                  filename, line);
   }
   else
   {
      dump_update(p_dump,
                  "Destroy fail (%s, %ld)\n"
                  "- candidate hasn't been inited\n",
                  filename, line);
   }
   
   p_dump->init_line = -1;
   p_dump->ban_line  = -1;
   
   p_dump->prevCaller = CALLER_STACK_DESTROY;
   
   return 0;
}

void stack_dump(const Stack *const p_stack)
{
   Dump* p_dump = find_dump(p_stack);
   
   if (p_dump == nullptr)
      printf("\nNo info about stack with address (%p)\n", p_stack);
   else
   {
      printf("%s"
             "================================\n", p_dump->log);
   }
}

void stack_info(const Stack *const p_stack)
{
   printf("\n"
          "=======================================================\n");
   
   printf("Stack address:         %#p\n"
          "Min capacity:          %zu\n"
          "Capacity:              %zu\n"
          "Size:                  %zu\n"
          #ifdef STK_CANARY_PROTECT
          "Bytes:                 %zu\n"
          #endif
          "Storage address:       %#p\n",
          p_stack,
          p_stack->minCapacity,
          p_stack->capacity,
          p_stack->size,
#ifdef STK_CANARY_PROTECT
          p_stack->bytes,
#endif
          p_stack->storage);

#ifdef STK_CANARY_PROTECT
   printf("-------------------------------------------------------\n");
   printf("Stack left  canary:    %zx\n"
          "Stack right canary:    %zx\n",
          p_stack->leftCanary,
          p_stack->rightCanary);
   
   if (p_stack->storage)
      printf("\n"
             "Storage left  canary:  %zx\n"
             "Storage right canary:  %zx\n",
             *(canary_t *)find_storage_leftCanary(p_stack->storage),
             *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes));
#endif

#ifdef STK_HASH_PROTECT
   hash_t verifyHash = 0;
   calc_stack_hash(p_stack, &verifyHash);
   
   printf("-------------------------------------------------------\n");
   printf("Stack  hash:           %zu\n"
          "Verify hash:           %zu\n",
          p_stack->hash,
          verifyHash);
#endif
   
   if (p_stack->storage)
   {
      printf("-------------------------------------------------------\n");
      
      static element_t *poison = nullptr;
      
      if (poison == nullptr)
      {
         poison = (element_t *)malloc(sizeof(element_t));
         
         memset(poison, STK_POISON, sizeof(element_t));
      }
      
      element_t *p_element     = nullptr;
      element_t *p_lastElement = nullptr;

#ifdef STK_CANARY_PROTECT
      p_element     = (element_t *)((canary_t *)find_storage_leftCanary(p_stack->storage) + 1);
      p_lastElement = (element_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) - 1;
#else
      p_element     = (element_t *)p_stack->storage;
            p_lastElement = (element_t *)p_stack->storage + p_stack->capacity - 1;
#endif
      
      for (size_t i = 0; p_element <= p_lastElement; p_element++, i++)
      {
         printf("Container[%4zu]:       ", i);
         
         if (*p_element == *poison)
            printf("poison\n");
         else
            printf("%" SPECIFIER "\n", *p_element);
      }
   }
   printf("=======================================================\n");
}


bitmask_t stack_log(const Stack *const p_stack, const StackLogCallers caller,
               const char *const filename, const long line, const bitmask_t bitmask, ...)
{
   if (p_stack != nullptr)
   {
      Dump* p_dump = get_dump(p_stack);
      
      va_list ap = {};
      va_start(ap, bitmask);
      
      switch (caller)
      {
         case CALLER_STACK_INIT:
         {
            process_init(p_dump, filename, line, bitmask, ap);

            break;
         }
      
         case CALLER_STACK_PUSH:
         {
            process_push(p_dump, filename, line, bitmask, ap);

            break;
         }

         case CALLER_STACK_POP:
         {
            process_pop(p_dump, filename, line, bitmask, ap);

            break;
         }

         case CALLER_STACK_DESTROY:
         {
            process_destroy(p_dump, filename, line, bitmask, ap);
            
            break;
         }
      
         default:
         {
            break;
         }
      }
   
      va_end(ap);
   }

   return bitmask;
}
#endif // STK_DEBUG
#endif // STK_PRODUCTION