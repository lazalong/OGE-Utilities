#pragma once
/*
 *  Basic C Memory allocator with leak reporting
 *
 *  Copyright (c) 2023 Steven Gay (lazalong@gmail.com)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
#include "../Oge.h" // In which '#DEFINE OGE_USE_LEAK_CHECK 1' can be set

 /**
   Usage:
     - Library must be build with preprocessor definition: OGE_MEMORY_IMPLEMENTATION
     - Set preprocessor OGE_USE_LEAK_CHECK to 1 to use the leak report;
       otherwise the standard malloc/calloc/realloc/free are used
     - Call OgeMemoryReport(1); when you want a report.
     void main()
     {
         OgeMemoryReport(1);
     }
 */
#ifdef OGE_MEMORY_IMPLEMENTATION
#undef OGE_MEMORY_IMPLEMENTATION

 // if we've already included oge memory before, undefine the macros
#ifdef malloc
#   undef malloc
#   undef calloc
#   undef free
#   undef realloc
#endif

#include <stdlib.h>

#   if !OGE_USE_LEAK_CHECK

// inlining to avoid link issue: https://stackoverflow.com/questions/19148639/already-defined-obj-linking-error
inline void* OgeMalloc(size_t size)
{
    int* ptr = malloc(size);
    assert(ptr != 0);
    return ptr;
}

inline void* OgeCalloc(size_t size)
{
    int* ptr = calloc(1, size);
    assert(ptr != 0);
    return ptr;
}

inline void* OgeRealloc(void* obj, size_t size)
{
    int* ptr = realloc(obj, size);
    assert(ptr != 0);
    return ptr;
}

inline void OgeFree(void* obj)
{
    free(obj);
}

inline void OgeMemoryReport(int showAll)
{
    printf("No report because preprocessor OGE_USE_LEAK_CHECK was not set to 1.\n");
}

#   else // OGE_USE_LEAK_CHECK

#include <string.h> // for memcpy

typedef struct OgeMallocInfo OgeMallocInfo;

struct OgeMallocInfo
{
    OgeMallocInfo* next;
    OgeMallocInfo* prev;
    int line;
    const char* file;
    size_t size;
};

static OgeMallocInfo* _mallocInfoHead;
static size_t MallocInfoSize = sizeof(OgeMallocInfo);

// inlining to avoid link issue: https://stackoverflow.com/questions/19148639/already-defined-obj-linking-error
inline void* OgeMalloc(size_t size, const char* file, int line)
{
    OgeMallocInfo* ptr = (OgeMallocInfo*)malloc(size + MallocInfoSize);
    assert(ptr != 0);
    if (ptr == NULL) return 0;

    ptr->file = file;
    ptr->line = line;

    if (_ogeLogger != NULL) {
        LOGA(0, "add", (unsigned long)(ptr), size, file, line);
    }

    ptr->next = _mallocInfoHead;
    if (_mallocInfoHead)
        ptr->next->prev = ptr;
    ptr->prev = NULL;
    ptr->size = size;
    _mallocInfoHead = ptr;
    return ptr + 1;
}

inline void* OgeCalloc(size_t size, const char* file, int line)
{
    OgeMallocInfo* ptr = (OgeMallocInfo*)calloc(1, size + MallocInfoSize);
    assert(ptr != 0);
    if (ptr == NULL) return 0;

    ptr->file = file;
    ptr->line = line;

    // TODO ptr->callstackStr = callstack()/StackWalk64()  so we know where the alloc has been called when in a lib struct such as Str!

    ptr->next = _mallocInfoHead;
    if (_mallocInfoHead)
        ptr->next->prev = ptr;
    ptr->prev = NULL;
    ptr->size = size;
    _mallocInfoHead = ptr;

    if (_ogeLogger != NULL) {
        LOGA(0, "add", (unsigned long)(ptr), size, file, line);
    }

    return ptr + 1;
}

inline void OgeFree(void* obj)
{
    if (obj == NULL)
        return;

    OgeMallocInfo* mi = (OgeMallocInfo*)obj - 1;

    if (_ogeLogger != NULL && _ogeLogger->logFile != 0) {
        LOGA(0, "del", (unsigned long)(mi), mi->size, mi->file, mi->line);
    }

    mi->size = ~mi->size; // flipps the bits
    if (mi->prev != NULL)
        mi->prev->next = mi->next;
    if (mi->next)
        mi->next->prev = mi->prev;

    if (_mallocInfoHead == mi)
        _mallocInfoHead = mi->next;

    free(mi);
}

inline void* OgeRealloc(void* obj, size_t size, const char* file, int line)
{
    if (obj == NULL)
    {
        return OgeMalloc(size, file, line);
    }
    else if (size == 0)
    {
        OgeFree(obj);
        obj = NULL;
        return NULL;
    }
    else
    {
        // LATER What should we do with the original file/line? Concatenate?

        OgeMallocInfo* omi = (OgeMallocInfo*)obj - 1;
        if (size <= omi->size)
        {
            return obj;
        }
        else
        {
            void* ptr = OgeMalloc(size, file, line);
            if (ptr)
            {
                memcpy(ptr, obj, omi->size);
                OgeFree(obj);
                obj = NULL;
            }
            return ptr;
        }
    }
}

// LATER fprintf version
inline void OgeInternalPrint(const char* str, OgeMallocInfo* omi)
{
#if defined(_MSC_VER)
    printf("%s: %s (%4d) : %16lld bytes at %p\n", str, omi->file, omi->line, (long long)omi->size, (void*)(omi + 1));
#else
#   if defined __MINGW32__
    // TODO
#   else // GCC
    // TODO
#   endif

#endif
}

inline void OgeMemoryReport(int showAll)
{
    printf("\n======  Memory Report ============\n");

    OgeMallocInfo* omi = _mallocInfoHead;
    while (omi)
    {
        if ((ptrdiff_t)omi->size >= 0)
            OgeInternalPrint("LEAK!", omi);
        omi = omi->next;
    }

    if (showAll != 1)
        return;

    omi = _mallocInfoHead;
    while (omi)
    {
        if ((ptrdiff_t)omi->size < 0)
            OgeInternalPrint("Freed", omi);
        omi = omi->next;
    }

    printf("\n======  End Memory Report ============\n");
}

#   endif // OGE_USE_LEAK_CHECK
#endif // OGE_MEMORY_IMPLEMENTATION

// ----------------- Declaration ------------------------

#if !defined(INCLUDE_OGE_MEMORY_H) || !defined(malloc)
#   define INCLUDE_OGE_MEMORY_H
#   include <stdlib.h>

#   if !OGE_USE_LEAK_CHECK

#       define malloc(size)        OgeMalloc(size);
#       define calloc(size)        OgeCalloc(size);
#       define realloc(obj, size)  OgeRealloc(obj, size);
#       define free(obj)           OgeFree(obj);

extern void* OgeMalloc(size_t size);
extern void* OgeCalloc(size_t size);
extern void* OgeRealloc(void* obj, size_t size);
extern void  OgeFree(void* obj);
extern void  OgeMemoryReport(int showAll);
// char* OgeMemoryReportString(int showAll);

#ifdef __cplusplus
void * operator new(size_t size) {
    return OgeMalloc(size);
}

void operator delete(void* p) {
    OgeFree(p);
}
#endif

#   else // OGE_USE_LEAK_CHECK

#       include <stdlib.h> // to avoid lots of errors

#       define malloc(size)        OgeMalloc(size, __FILE__, __LINE__);
#       define calloc(size)        OgeCalloc(size, __FILE__, __LINE__);
#       define realloc(obj, size)  OgeRealloc(obj, size, __FILE__, __LINE__);
#       define free(obj)           OgeFree(obj);

extern void* OgeMalloc(size_t size, const char* file, int line);
extern void* OgeCalloc(size_t size, const char* file, int line);
extern void* OgeRealloc(void* obj, size_t size, const char* file, int line);
extern void  OgeFree(void* obj);
extern void  OgeMemoryReport(int showAll);
// TODO char* OgeMemoryReportString(int showAll);

#ifdef OGE_MEMORY_IMPLEMENTATION

#ifdef __cplusplus
void* operator new(size_t size) {
    return OgeMalloc(size, __FILE__, __LINE__);
}

void operator delete(void* p) {
    OgeFree(p);
}
#endif

#endif // ! OGE_MEMORY_IMPLEMENTATION

#   endif // OGE_USE_LEAK_CHECK
#endif // INCLUDE_OGE_MEMORY_H
