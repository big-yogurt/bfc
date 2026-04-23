/*
    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <https://unlicense.org/>
*/

/*
    Dynamic ARRay
    
    Inspired by nob.h (https://github.com/tsoding/nob.h)

    This library provides macro for working with structures that
    implemented a dynamic array. They must have the required fields items,
    len, cap.

    struct A {
        // required fields
        Type* items;
        size_t cap;
        size_t len;

        // other fields
    };

    items - pointer to items
    cap - allocated memory (capacity)
    len - count of items (length)

    DARR_DEF_CAP - default capacity

    darr_append(darr, item)
    darr_pop(darr)
    darr_reserve(darr, new_cap)
*/

/* You can define your own default capacity */
#ifndef DARR_DEF_CAP
    #define DARR_DEF_CAP 15
#endif /* DARR_DEF_CAP */

#ifndef DARR_ALLOC
    #include <stdlib.h>
    #define DARR_REALLOC realloc
#endif /* DARR_ALLOC */

#ifndef DARR_FREE
    #define DARR_FREE free
#endif /* DARR_FREE */

#define panic(fmt, ...) \
    do { \
        fprintf(stderr, "[PANIC] "fmt"\n", ##__VA_ARGS__); \
        exit(-1); \
    } while (0)

#define panic_if(expr, fmt, ...) \
    do { \
        if (expr) { \
            panic(fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define darr_reserve(darr, expected_cap) \
    do { \
        if (expected_cap > (darr)->cap) { \
            if (0 == (darr)->cap) { \
                (darr)->cap = DARR_DEF_CAP; \
            } \
            while (expected_cap > (darr)->cap) { \
                (darr)->cap *= 2; \
            } \
            (darr)->items = DARR_REALLOC( \
                (darr)->items, \
                (darr)->cap * sizeof(*(darr)->items) \
            ); \
            panic_if(NULL == (darr)->items, "Failed to allocate memory"); \
        } \
    } while (0)

#define darr_append(darr, item) \
    do { \
        darr_reserve(darr, (darr)->len + 1); \
        (darr)->items[(darr)->len++] = item; \
    } while (0)

#define darr_append_many(darr, new_items, new_items_count) \
    do { \
        darr_reserve((darr), (darr)->len + (new_items_count)); \
        memcpy((darr)->items + (darr)->len, (new_items), (new_items_count)*sizeof(*(darr)->items)); \
        (darr)->len += (new_items_count); \
    } while (0)

/*
    It's like `foreach (type x in darr)`
*/
#define darr_foreach(type, x, darr) \
    for (type* x = (darr)->items; x < (darr)->items + (darr)->len; x++)

#define darr_free(darr) \
    do { \
        DARR_FREE((darr)->items); \
        (darr)->cap = 0; \
        (darr)->len = 0; \
    } while (0)

#define darr_pop(darr) \
    ( \
        ({panic_if(0 == (darr)->len, "Failed to pop item");}), \
        (darr)->items[--(darr)->len] \
    )
