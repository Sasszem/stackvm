// Error statuses:

struct Error
{
        cell code;
        addr location;
        cell param;
};

#define ERROR_NO_ERROR 0
#define ERROR_DIV_BY_ZERO 1
#define ERROR_UNKNOWN_OPCODE 2
#define ERROR_MOD_BY_ZERO 3
#define ERROR_ILL_ADR 4
#define ERROR_STACK_UNDERFLOW 5
#define ERROR_STACK_OVERFLOW 6
#define ERROR_CALL_STACK_OVERFLOW 7
#define ERROR_CALL_STACK_UNDERFLOW 8
