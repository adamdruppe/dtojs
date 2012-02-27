
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef void __d_void;
typedef int8_t __d_byte;
typedef uint8_t __d_ubyte;
typedef int16_t __d_short;
typedef uint16_t __d_ushort;
typedef int32_t __d_int;
typedef uint32_t __d_uint;

typedef struct __d_Array __d_Array;

struct __d_Array
{
    __d_void* ptr;
    __d_uint length;
};

__d_Array __d_array(__d_void* ptr, __d_uint length)
{
    __d_Array arr;
    arr.ptr = ptr;
    arr.length = length;
    return arr;
}

void __d_assert_msg(__d_Array msg, __d_Array file, __d_uint line)
{
    fprintf(stderr, "%.*s(%d): %.*s\n", file.ptr, file.length, line, msg.ptr, msg.length);
    exit(1);
}

void __d_assert(__d_Array file, __d_uint line)
{
    fprintf(stderr, "%.*s(%d): Assertion failure\n", file.ptr, file.length, line);
    exit(1);
}

__d_int _Dmain(__d_Array args);

void main()
{
    _Dmain(__d_array(NULL, 0));
}
