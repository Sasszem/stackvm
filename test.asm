#define $N 10

// If you can guess what the following program does
// I owe you a "SZAKADT TESZKÓS SZATYOR"

push 2
loop:
    push &tick call
    push 1 +
    dup push $N =
    push &loop jz 
drop
push &b load
halt
a:
    1
b:
    1

tick:
    push &a load
    push &b load
    dup rot + swap
    push &a store
    push &b store
ret

