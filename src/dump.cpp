

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include "../include/dump.h"

#ifndef STK_PRODUCTION
#ifdef  STK_DEBUG
typedef stk_element_t element_t;

typedef stk_bitmask_t bitmask_t;

#ifdef STK_CANARY_PROTECT
typedef stk_canary_t  canary_t;
#endif

typedef int_fast8_t error_t;

struct StackDump
{
   const Stack *p_stack;
   char*        log;
   size_t       logSize;
};

struct StackDebugger
{
   const Stack *p_stack;
   Stack        copyStack;
   
   const char *init_filename;
   long        init_line;
};

//*===============================================================================*
//| Stack name:    (stack name)                                                   |
//| Stack index:   (stack name)                                                   |
//| Stack address: (stack address)                                                |
//|-------------------------------------------------------------------------------|
//|  CALLER   |  ARGUMENT                         |  FROM                         |
//|-------------------------------------------------------------------------------|
//$  init     |  minCapacity: 2                   |  10, P:/Documents/stack.cpp   $
//+  push     |  element:     0                   |  11, P:/Documents/stack.cpp   +
//+  push     |  element:     7                   |  15, P:/Documents/stack.cpp   +
//+  push     |  element:     7                   |  15, P:/Documents/stack.cpp   +
//-  pop      |  p_output:    0x00141fxe13000000  |  20, P:/Documents/stack.cpp   -
//#  destroy  |  -                                |  24, P:/Documents/stack.cpp   #
//+  push     |  element:     707                 |  29, P:/Documents/stack.cpp   +
//|-------------------------------------------------------------------------------|
//! ERROR!                                                                        !
//! Trying to push to uninitialized stack at (line) line.                         !
//|-------------------------------------------------------------------------------|
//? WARNING!                                                                      ?
//? stack_push won't' return bitmask with information,                            ?
//? that you are pushing to uninitialized or banned stack.                        ?
//? Use #define STK_ANTI_FOOL_PROTECT in config file to prevent such faults.      ?
//*===============================================================================*

static const size_t MAX_FORMATTED_STRING_SIZE = 2048 - 1;
static       size_t dumps_counter             = 0;
static       size_t debuggers_counter         = 0;

static StackDump      *dumps                     = nullptr;
static StackDump      *p_prevDump                = nullptr;
static StackDebugger  *debuggers             = nullptr;
static StackDebugger  *p_prevDebugger        = nullptr;

static char  templog[MAX_FORMATTED_STRING_SIZE + 1] = "";
static bool  is_dump_registered = false;


static error_t update_log(StackDump *const p_dump, const char *const format, ...)
{
   va_list ap = {};
   va_start(ap, format);

   p_dump->logSize += vsprintf(templog, format, ap) + 1;
   p_dump->log      = (char *)realloc(p_dump->log, p_dump->logSize * sizeof(char));

   strcat(p_dump->log, templog);

   printf(p_dump->log);
   va_end(ap);

   return 0;
}

static StackDump* find_dump(const Stack *const p_stack)
{
   if (p_prevDump && p_prevDump->p_stack == p_stack)
      return p_prevDump;
   
   for (size_t i = 0; i < dumps_counter; i++)
      if (dumps[i].p_stack == p_stack)
         return (StackDump *)dumps + i;

   return nullptr;
}

static StackDump* create_dump(const Stack *const p_stack)
{
   dumps_counter += 1;
   dumps = (StackDump *)realloc(dumps, sizeof(StackDump) * dumps_counter);

   StackDump* const p_dump = (StackDump *)dumps + dumps_counter - 1;

   p_dump->p_stack      =  p_stack;
   
   p_dump->log          =  (char *)calloc(128, sizeof(char));
   p_dump->logSize      =  0;
   
   update_log(p_dump,
              "*============================================================================\n"
              "| Stack index:    %zu\n"
              "| Stack address:  %#p\n"
              "|----------------------------------------------------------------------------\n"
              "|  CALLER   |  ARGUMENT              |  LINE   |  FILENAME\n"
              "|-----------|------------------------|---------|-----------------------------\n",
              dumps_counter - 1, p_stack);

   return p_dump;
}

static StackDump* get_dump(const Stack *const p_stack)
{
   StackDump* p_dump = find_dump(p_stack);

   if (p_dump == nullptr)
      p_dump = create_dump(p_stack);

   return p_dump;
}

static StackDebugger* find_debugger(const Stack *const p_stack)
{
   if (p_prevDebugger && p_prevDebugger->p_stack == p_stack)
      return p_prevDebugger;
   
   for (size_t i = 0; i < debuggers_counter; i++)
      if (debuggers[i].p_stack == p_stack)
         return (StackDebugger *)debuggers + i;
   
   return nullptr;
}

static StackDebugger* create_debugger(const Stack *const p_stack)
{
   debuggers_counter += 1;
   debuggers = (StackDebugger *)realloc(dumps, sizeof(StackDebugger) * debuggers_counter);
   
   StackDebugger* const p_debugger = (StackDebugger *)debuggers + debuggers_counter - 1;
   
   p_debugger->copyStack     = *p_stack;
   p_debugger->p_stack       =  p_stack;
   p_debugger->init_line     = -1;
   p_debugger->init_filename = nullptr;
   
   return p_debugger;
}

static StackDebugger* get_debugger(const Stack *const p_stack)
{
   StackDebugger* p_debugger = find_debugger(p_stack);
   
   if (p_debugger == nullptr)
      p_debugger = create_debugger(p_stack);
   
   return p_debugger;
}

void stack_debug(const Stack *const p_stack)
{
   if (p_stack != nullptr)
   {
      get_debugger(p_stack);
   }
}

static void report(const StackDebugger *const p_debugger, const char *const format, ...)
{
   va_list ap = {};
   va_start(ap, format);
   
   printf("*===============================================================================*\n"
          "| Stack index:    %zu                                                           |\n"
          "| Stack address:  %zx                                                           |\n"
          "|-------------------------------------------------------------------------------|\n",
          debuggers_counter - 1, p_debugger->p_stack);
   vprintf(format, ap);
   printf("*===============================================================================*\n");
   
   va_end(ap);
   exit(EXIT_FAILURE);
}

static error_t compare_stacks(const Stack *const p_stack, const Stack *const p_copyStack)
{
   size_t size  = 0;
   
   if (p_stack->size != p_copyStack->size)
   {
      size += sprintf(templog + size,
                      "Stack size has was corrupted.\n"
                      "Size have to be:  %zu\n"
                      "Size of stack is: %zu\n",
                      p_copyStack->size,
                      p_stack->size);
      
   }
   
   if (p_stack->capacity != p_copyStack->capacity)
   {
      size += sprintf(templog + size,
                      "Stack capacity has was corrupted.\n"
                      "Capacity have to be:  %zu\n"
                      "Capacity of stack is: %zu\n",
                      p_copyStack->capacity,
                      p_stack->capacity);
   }
   
   if (p_stack->minCapacity != p_copyStack->minCapacity)
   {
      size += sprintf(templog + size,
                      "Stack minimal capacity has was corrupted.\n"
                      "Minimal capacity have to be:  %zu\n"
                      "Minimal capacity of stack is: %zu\n",
                      p_copyStack->minCapacity,
                      p_stack->minCapacity);
   }
   
   #ifdef STK_CANARY_PROTECT
   if (p_stack->leftCanary != STK_CANARY)
   {
      size += sprintf(templog + size,
                      "Stack left canary was attacked.\n"
                      "Canary have to be: %zu\n"
                      "Canary of stack is: %zu\n",
                      STK_CANARY, p_stack->leftCanary);
   }
   
   if (p_stack->rightCanary != STK_CANARY)
   {
      size += sprintf(templog + size,
                      "Stack right canary was attacked.\n"
                      "Canary have to be: %zu\n"
                      "Canary of stack is: %zu\n",
                      STK_CANARY, p_stack->rightCanary);
   }
   
   if (p_stack->storage != p_copyStack->storage)
   {
      size += sprintf(templog + size,
                      "Stack storage was attacked.\n"
                      "Storage have to be: %zu\n"
                      "Storage of stack is: %zu\n",
                      p_copyStack->storage, p_stack->storage);
   }
   else if (p_stack->storage != nullptr)
   {
      if (*(canary_t *)find_storage_leftCanary(p_stack->storage) != STK_CANARY)
      {
         size += sprintf(templog + size,
                         "Storage left canary was attacked.\n"
                         "Storage left canary have to be: %zu\n"
                         "Storage left canary of stack is: %zu\n",
                         STK_CANARY, *(canary_t *)find_storage_leftCanary(p_stack->storage));
      }
      
      if (*(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) != STK_CANARY)
      {
         size += sprintf(templog + size,
                         "Storage right canary was attacked.\n"
                         "Storage right canary have to be: %zu\n"
                         "Storage right canary of stack is: %zu\n",
                         STK_CANARY, *(canary_t *)find_storage_leftCanary(p_stack->storage));
      }
   }
   
   
   if (p_stack->bytes != p_copyStack->bytes)
   {
      size += sprintf(templog + size,
                      "Stack bytes has was corrupted.\n"
                      "Bytes have to be:  %zu\n"
                      "Bytes of stack is: %zu\n",
                      p_copyStack->bytes,
                      p_stack->bytes);
   }
   #endif
   
   #ifdef STK_HASH_PROTECT
   if (p_stack->hash != p_copyStack->hash)
   {
      size += sprintf(templog + size,
                      "Stack hash has was corrupted.\n"
                      "hash have to be:  %zu\n"
                      "hash of stack is: %zu\n",
                      p_copyStack->hash,
                      p_stack->hash);
   }
   #endif
   
   return size;
}


void dump()
{
   if (dumps_counter >= 1)
   {
      FILE  *logfile = fopen(STK_LOGFILE_PATH, "w");
      
      char config[400] = "Protection:  "
                               #ifdef STK_UPROTECT
                               "#define STK_UNPROTECT\n"
                               #endif
                               
                               #ifdef STK_ANTI_FOOL_PROTECT
                               "STK_ANTI_FOOL_PROTECT  "
                               #endif
   
                               #ifdef STK_CANARY_PROTECT
                               "STK_CANARY_PROTECT  "
                               #endif
   
                               #ifdef STK_HASH_PROTECT
                               "STK_CANARY_PROTECT  "
                               #endif
                               
                               "\n"
                               "Element specifier:  "
                               #ifdef STK_ELEMENT_SPECIFIER
                               STK_ELEMENT_SPECIFIER
                               #else
                               "none"
                               #endif
                               "\n";
   
      sprintf(config + strlen(config),
                      "Limits:  STK_MIN_CAPACITY  = %zu   STK_MAX_CAPACITY  = %zu\n"
                      "         STK_MIN_CAPACITY_ = %zu   STK_MAX_CAPACITY_ = %zu\n",
                      STK_MIN_CAPACITY, STK_MAX_CAPACITY, STK_MIN_CAPACITY_, STK_MAX_CAPACITY_);
      
      fwrite(config, sizeof(char), strlen(config), logfile);
      
      for (size_t i = 0; i < dumps_counter; i++)
      {
         fwrite(dumps[i].log, sizeof(char), dumps[i].logSize, logfile);
      }
      
      fclose(logfile);
   }
}

static void register_dump()
{
   if (is_dump_registered == false)
   {
      atexit(dump);
      is_dump_registered = true;
   }
}

bitmask_t process_init(Stack *const p_stack, const size_t userMinCapacity,
                            const char *const filename, const long line)
{
   register_dump();

   StackDebugger *p_debugger = find_debugger(p_stack);
   StackDump *p_dump = get_dump(p_stack);
   bitmask_t bitmask = 0;

   if (p_debugger != nullptr)
   {
      if (p_debugger->init_line >= 0)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Stack has already been initialized at %ld, %s.\n",
                line, filename, p_debugger->init_line, p_debugger->init_filename);
      }

      if (userMinCapacity > STK_MAX_CAPACITY_)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Incorrect minimal capacity %zu.\n",
                line, filename, userMinCapacity);
      }

      if (stack_init_(p_stack, userMinCapacity) != 0)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Your system couldn't allocate enough memory for stack.\n",
                line, filename);
      }
      else
      {
         update_log(p_dump,
                    "$  init     |  %-20zu  |  %-5ld  |  %s\n",
                    userMinCapacity, line, filename);
      }

      p_debugger->copyStack     = *p_stack;
      p_debugger->init_filename = filename;
      p_debugger->init_line     = line;

      return 0;
   }

   bitmask = stack_init_(p_stack, userMinCapacity);

   if (bitmask == 0)
   {
      update_log(p_dump,
                 "$  init     |  %-20zu  |  %-5ld  |  %s\n",
                 userMinCapacity, line, filename);
   }
   else
   {
      update_log(p_dump,
                 ">$ init     |  %-20zu  |  %-5ld  |  %s\n",
                 userMinCapacity, line, filename);
   }

   return bitmask;
}

bitmask_t process_push(Stack *const p_stack, const element_t element,
                       const char *const filename, const long line)
{
   register_dump();
   
   StackDebugger *p_debugger = find_debugger(p_stack);
   StackDump *p_dump = get_dump(p_stack);
   bitmask_t bitmask = 0;
   
   if (p_debugger != nullptr)
   {
      if (p_debugger->init_line == -1)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Pushing to uninitialized stack.\n",
                line, filename);
      }
   
      if (compare_stacks(p_stack, &p_debugger->copyStack))
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "%s\n",
                line, filename, templog);
      }
      
      if (p_stack->size == STK_MAX_CAPACITY_)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Stack is already full.\n",
                line, filename);
      }
   
      if (stack_push_(p_stack, element) != 0)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Your system couldn't allocate enough memory for stack.\n",
                line, filename);
      }
      else
      {
         update_log(p_dump,
                    "+  push     |  %-20" SPECIFIER "  |  %-5ld  |  %s\n",
                    element, line, filename);
      }
      p_debugger->copyStack = *p_stack;
      
      return 0;
   }
   
   bitmask = stack_push_(p_stack, element);
   
   if (bitmask == 0)
   {
      update_log(p_dump,
                 "+  push     |  %-20" SPECIFIER "  |  %-5ld  |  %s\n",
                 element, line, filename);
   }
   else
   {
      update_log(p_dump,
                 ">+ push     |  %-20" SPECIFIER "  |  %-5ld  |  %s\n",
                 element, line, filename);
   }
   
   return bitmask;
}

bitmask_t process_pop(Stack *const p_stack, element_t *const p_output,
                       const char *const filename, const long line)
{
   register_dump();
   
   StackDebugger *p_debugger = find_debugger(p_stack);
   StackDump *p_dump = get_dump(p_stack);
   bitmask_t bitmask = 0;
   
   if (p_debugger != nullptr)
   {
      if (p_debugger->init_line == -1)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Popping from uninitialized stack.\n",
                line, filename);
      }
      
      if (p_output == nullptr)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Popping to null pointer.\n",
                line, filename);
      }
      
      if (compare_stacks(p_stack, &p_debugger->copyStack))
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "%s\n",
                line, filename, templog);
      }
      
      if (p_stack->size == 0)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Stack is empty.\n",
                line, filename);
      }
      
      if (stack_pop_(p_stack, p_output) != 0)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Your system couldn't allocate enough memory for stack.\n",
                line, filename);
      }
      else
      {
         update_log(p_dump,
                    "-  pop      |  %-#20p  |  %-5ld  |  %s\n",
                    p_output, line, filename);
      }
      
      p_debugger->copyStack = *p_stack;
      
      return 0;
   }
   
   bitmask = stack_pop_(p_stack, p_output);
   
   if (bitmask == 0)
   {
      update_log(p_dump,
                 "-  pop      |  %-#20p  |  %-5ld  |  %s\n",
                 p_output, line, filename);
   }
   else
   {
      update_log(p_dump,
                 ">- pop      |  nullptr               |  %-5ld  |  %s\n",
                 line, filename);
   }
   
   return bitmask;
}

void process_destroy(Stack *const p_stack,
                       const char *const filename, const long line)
{
   register_dump();
   
   StackDump *p_dump = get_dump(p_stack);
   StackDebugger *p_debugger = find_debugger(p_stack);
   bitmask_t bitmask = 0;
   
   if (p_debugger != nullptr)
   {
      if (p_debugger->init_line == -1)
      {
         report(p_debugger,
                "ERROR AT %ld, %s.\n"
                "Trying to destroy uninitialized stack.\n",
                line, filename);
      }
      
      stack_destroy_(p_stack);
      
      p_debugger->init_line = -1;
      p_debugger->init_filename = nullptr;
   
   }
   
   stack_destroy_(p_stack);
   
   update_log(p_dump,
              "#  destroy  |  -                     |  %-5ld  |  %s\n",
              line, filename);
}


//static int process_push(StackDump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
//{
//   if (p_dump->ban_line == -1)
//   {
//      if (p_dump->prevCaller != CALLER_STACK_PUSH)
//      {
//         update_log(p_dump,
//                    "------------------------------\n");
//      }
//      else
//      {
//         update_log(p_dump, "\n");
//      }
//
//      if (bitmask == 0)
//      {
//         update_log(p_dump,
//                    "Push (%" SPECIFIER ") success (%s, %ld)\n",
//                    va_arg(ap, element_t), filename, line);
//
//         if (p_dump->init_line == -1)
//         {
//            update_log(p_dump,
//                       "WARNING! This stack hasn't been inited yet,\n"
//                       "         but stack_push won't signal about it.\n"
//                       "         Stack may work incorrectly\n");
//         }
//      }
//      else
//      {
//         update_log(p_dump,
//                    "Push (%" SPECIFIER ") fail (%s, %ld)\n",
//                    va_arg(ap, element_t), filename, line);
//
//         if (bitmask & StackDebug::BAD_STATEMENT_WITH_BANNED)
//         {
//            if (p_dump->init_line >= 0)
//            {
//               update_log(p_dump,
//                          "- stack has been broken by changing it's fields\n");
//            }
//            else
//            {
//               update_log(p_dump,
//                          "- stack hasn't been inited yet\n");
//            }
//         }
//
////         if (bitmask & StackDetails::LEFT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- stack left canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::RIGHT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- stack right canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::STORAGE_LEFT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- storage left canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::STACK_FULL)
////         {
////            update_log(p_dump,
////                        "- storage full with size (%%zu)\n",
////                        p_dump->p_stack->size);
////         }
////
////
////         if (bitmask & StackDetails::STORAGE_RIGHT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- storage right canary attacked.\n");
////         }
//
//         if (bitmask & StackDebug::MEMORY_NOT_ALLOCATED)
//         {
//            update_log(p_dump,
//                       "- memory for hash calculating wasn't allocated\n");
//         }
//
//         if (bitmask & StackDebug::HASH_NOT_VERIFIED)
//         {
//            hash_t verifyHash = 0;
//            calc_stack_hash(p_dump->p_stack, &verifyHash);
//
//            update_log(p_dump,
//                       "- hash wasn't verified\n"
//                       "  stack  hash: (%zu)\n"
//                       "  verify hash: (%zu)\n",
//                       p_dump->p_stack->hash,
//                       verifyHash);
//         }
//
//         if (bitmask & StackDebug::STORAGE_NOT_UPDATED)
//         {
//            update_log(p_dump,
//                       "- storage wasn't updated\n");
//         }
//
//      }
//
//      p_dump->prevCaller = CALLER_STACK_PUSH;
//   }
//
//   return 0;
//}
//
//static int process_pop(StackDump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
//{
//   if (p_dump->ban_line == -1)
//   {
//      if (p_dump->prevCaller != CALLER_STACK_POP)
//      {
//         update_log(p_dump,
//                    "------------------------------\n");
//      }
//      else
//      {
//         update_log(p_dump, "\n");
//      }
//
//      const element_t *p_output = va_arg(ap, element_t *);
//
//      if (bitmask == 0)
//      {
//         update_log(p_dump,
//                    "Pop (%" SPECIFIER ") to (%s) with address (%p) success (%s, %ld)\n",
//                    *p_output, va_arg(ap, char *), p_output, filename, line);
//
//         if (p_dump->init_line == -1)
//         {
//            update_log(p_dump,
//                       "WARNING! This stack hasn't been inited yet,\n"
//                       "         but stack_pop won't signal about it.\n"
//                       "         Stack may work incorrectly\n");
//         }
//      }
//      else
//      {
//         if (bitmask & StackDetails::STACK_EMPTY)
//         {
//            update_log(p_dump,
//                       "Pop nothing to (%s) with address (%p) fail (%s, %ld)\n"
//                       "- stack is empty\n",
//                       va_arg(ap, char *), p_output, filename, line);
//         }
//         else
//         {
//            update_log(p_dump,
//                       "Pop (%" SPECIFIER ") to (%s) with address (%p) fail (%s, %ld)\n",
//                       *p_output, va_arg(ap, char *), p_output, filename, line);
//
//         }
//
//         if (bitmask & StackDebug::BAD_OUTPUT_POINTER)
//         {
//            update_log(p_dump,
//                       "- output is null pointer\n");
//         }
//
//         if (bitmask & StackDebug::BAD_STATEMENT_WITH_BANNED)
//         {
//            if (p_dump->init_line >= 0)
//            {
//               update_log(p_dump,
//                          "- stack has been broken by changing it's fields\n");
//            }
//            else
//            {
//               update_log(p_dump,
//                          "- stack hasn't been inited yet\n");
//            }
//         }
//
////         if (bitmask & StackDetails::LEFT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- stack left canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::RIGHT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- stack right canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::STORAGE_LEFT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- storage left canary attacked.\n");
////         }
////
////         if (bitmask & StackDetails::STORAGE_RIGHT_CANARY_ATTACKED)
////         {
////            update_log(p_dump,
////                        "- storage right canary attacked.\n");
////         }
//
//         if (bitmask & StackDebug::MEMORY_NOT_ALLOCATED)
//         {
//            update_log(p_dump,
//                       "- memory for hash calculating wasn't allocated\n");
//         }
//
//         if (bitmask & StackDebug::HASH_NOT_VERIFIED)
//         {
//            hash_t verifyHash = 0;
//            calc_stack_hash(p_dump->p_stack, &verifyHash);
//
//            update_log(p_dump,
//                       "- hash wasn't verified\n"
//                       "  stack  hash: (%zu)\n"
//                       "  verify hash: (%zu)\n",
//                       p_dump->p_stack->hash,
//                       verifyHash);
//         }
//
//         if (bitmask & StackDebug::STORAGE_NOT_UPDATED)
//         {
//            update_log(p_dump,
//                       "- storage wasn't updated\n");
//         }
//
//      }
//
//      p_dump->prevCaller = CALLER_STACK_POP;
//   }
//
//   return 0;
//}
//
//static int process_destroy(StackDump *const p_dump, const char *const filename, const long line, const bitmask_t bitmask, va_list ap)
//{
//   if (p_dump->prevCaller != CALLER_STACK_DESTROY)
//   {
//      update_log(p_dump,
//                 "------------------------------\n");
//   }
//   else
//   {
//      update_log(p_dump, "\n");
//   }
//
//   if (p_dump->init_line >= 0)
//   {
//      update_log(p_dump,
//                 "Destroy success (%s, %ld)\n",
//                 filename, line);
//   }
//   else
//   {
//      update_log(p_dump,
//                 "Destroy fail (%s, %ld)\n"
//                 "- candidate hasn't been inited\n",
//                 filename, line);
//   }
//
//   p_dump->init_line = -1;
//   p_dump->ban_line  = -1;
//
//   p_dump->prevCaller = CALLER_STACK_DESTROY;
//
//   return 0;
//}
//
//void stack_dump(const Stack *const p_stack)
//{
//   StackDump* p_dump = find_dump(p_stack);
//
//   if (p_dump == nullptr)
//      printf("\nNo info about stack with address (%p)\n", p_stack);
//   else
//   {
//      printf("%s"
//             "================================\n", p_dump->log);
//   }
//}
//
//void stack_info(const Stack *const p_stack)
//{
//   printf("\n"
//          "=======================================================\n");
//
//   printf("Stack address:         %#p\n"
//          "Min capacity:          %zu\n"
//          "Capacity:              %zu\n"
//          "Size:                  %zu\n"
//          #ifdef STK_CANARY_PROTECT
//          "Bytes:                 %zu\n"
//          #endif
//          "Storage address:       %#p\n",
//          p_stack,
//          p_stack->minCapacity,
//          p_stack->capacity,
//          p_stack->size,
//#ifdef STK_CANARY_PROTECT
//          p_stack->bytes,
//#endif
//          p_stack->storage);
//
//#ifdef STK_CANARY_PROTECT
//   printf("-------------------------------------------------------\n");
//   printf("Stack left  canary:    %zx\n"
//          "Stack right canary:    %zx\n",
//          p_stack->leftCanary,
//          p_stack->rightCanary);
//
//   if (p_stack->storage)
//      printf("\n"
//             "Storage left  canary:  %zx\n"
//             "Storage right canary:  %zx\n",
//             *(canary_t *)find_storage_leftCanary(p_stack->storage),
//             *(canary_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes));
//#endif
//
//#ifdef STK_HASH_PROTECT
//   hash_t verifyHash = 0;
//   calc_hash(p_stack, &verifyHash);
//
//   printf("-------------------------------------------------------\n");
//   printf("Stack  hash:           %zu\n"
//          "Verify hash:           %zu\n",
//          p_stack->hash,
//          verifyHash);
//#endif
//
//   if (p_stack->storage)
//   {
//      printf("-------------------------------------------------------\n");
//
//      static element_t *poison = nullptr;
//
//      if (poison == nullptr)
//      {
//         poison = (element_t *)malloc(sizeof(element_t));
//
//         memset(poison, STK_POISON, sizeof(element_t));
//      }
//
//      element_t *p_element     = nullptr;
//      element_t *p_lastElement = nullptr;
//
//#ifdef STK_CANARY_PROTECT
//      p_element     = (element_t *)((canary_t *)find_storage_leftCanary(p_stack->storage) + 1);
//      p_lastElement = (element_t *)find_storage_rightCanary(p_stack->storage, p_stack->bytes) - 1;
//#else
//      p_element     = (element_t *)p_stack->storage;
//            p_lastElement = (element_t *)p_stack->storage + p_stack->capacity - 1;
//#endif
//
//      for (size_t i = 0; p_element <= p_lastElement; p_element++, i++)
//      {
//         printf("Container[%4zu]:       ", i);
//
//         if (*p_element == *poison)
//            printf("poison\n");
//         else
//            printf("%" SPECIFIER "\n", *p_element);
//      }
//   }
//   printf("=======================================================\n");
//}
//
//bitmask_t stack_log(const Stack *const p_stack, const StackLogCallers caller,
//               const char *const filename, const long line, const bitmask_t bitmask, ...)
//{
//   if (p_stack != nullptr)
//   {
//      StackDump* p_dump = get_dump(p_stack);
//
//      va_list ap = {};
//      va_start(ap, bitmask);
//
//      switch (caller)
//      {
//         case CALLER_STACK_INIT:
//         {
//            process_init(p_dump, filename, line, bitmask, ap);
//
//            break;
//         }
//
//         case CALLER_STACK_PUSH:
//         {
//            process_push(p_dump, filename, line, bitmask, ap);
//
//            break;
//         }
//
//         case CALLER_STACK_POP:
//         {
//            process_pop(p_dump, filename, line, bitmask, ap);
//
//            break;
//         }
//
//         case CALLER_STACK_DESTROY:
//         {
//            process_destroy(p_dump, filename, line, bitmask, ap);
//
//            break;
//         }
//
//         default:
//         {
//            break;
//         }
//      }
//
//      va_end(ap);
//   }
//
//   return bitmask;
//}

#endif // STK_DEBUG
#endif // STK_PRODUCTION