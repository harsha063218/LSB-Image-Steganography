#ifndef TYPES_H
#define TYPES_H

// User defined types
typedef unsigned int uint;

typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

#endif
