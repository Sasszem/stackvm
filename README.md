# Stack-VM
A stack-based VM for embedding

I designed and implemented this virtual machine for embedding purposes, for ex. in microcontrollers, using the Arduino IDE.
I have not embedded it in any Arduino project yet, but I plan to do that soon.

I used some ideas from https://github.com/cslarsen/stack-machine.


## Architechture
 - Stack-based
 - signed 16 bit (should be easily modifiable)
 - maximum 64K memory (depends on previous one)
 - currently 20 instructions (planning to add more)
 - 2 stacks (data and call)
 - error state

Memory is built up by `cell`s, which are `signed short` values (can be set in the config)
### Instructions
Instructions are in groups:

##### Stack group
- push - push the next cell on the stack
- drop - discard the top of the stack
- dup - duplicate the top of the stack
- swap - swap the first and second values on the stack
- rot - bring the third element to the top of the stack

#### Arithmetic group
- addition
- substraction
- multiplication
- division
- modulo

Division and modulo can cause the VM to error
 
#### Comparison group
 - equals
 - less than
 - greater than
 
All operators return 1 if true, 0 if false
LT and GT both operate in reverse, e.g they check the second value against the top of the stack.
For ex. push 5; push 3; > is true, as 5>3
This makes sense for comparing the top of the stack with a constant
BTW FORTH also does this

#### RAM group
 - store - stores the second value to the address specified by the top of the stack
 - load - loads a value from ram
  
#### Control flow group
 - goto
 - jump zero - checks the second value, if it's zero, jumps to the location on top of the stack
 - call - calls address on top of the stack
 - return - returns from call

#### IO group
 - inchar
 - innum
 - outchar
 - outnum

#### Misc group
 - halt - causes the machine to stop

## Instruction set implementation
Take a look at src/opcodes.hpp

## Configuration:
See `src/config.hpp`
You can configure:
 - RAM size
 - stack size
 - call stack size
 - stack underflow behaviour
   (error/0)
 - cell type

Note: With the current build config, the configuration is global to all instances built from the same object file.

Currently, stack, call stack, ram and the program itself are sharing the same memory with maximum size `VM_RAM_SIZE`, but that might change in the future.

## Errors
If an error occurs, the VM halts. You can get the error state with the `VM::getError()` function, which retuns an `Error*` :
```c++
struct Error
{
        cell code;
        addr location;
        cell param;
};
```

Error codes are defined in `src/error.hpp`, error types are:
 - no error (halted on halt instruction)
 - division by zero
 - unknown opcode - parameter is the opcode
 - modulo by zero
 - illegel address (jumping, RAM operations, etc.) - parameter is the address
 - stack underflow ( but can be disabled)
 - stack overflow
 - call stack overflow
 - call stack underflow
 
#### The supplied compiler

I wrote a simple compiler in Python2 for the machine
Features:
 - ASM-like syntax
 - FORTH-like syntax
 - Labels
 - __uses cpp(the c preprocessor)__
 - forward-referencing labels
 - easy2use command-line interface
 - pulls the opcodes from the hpp definitions to ensure compatibility
 
__C like preprocessor macros are supported__
 
Instructions are named like in this document, except:
 - addition, substraction, etc. are + - * / %
 - jump zero is jz
 - equals, greater & less are = > <

Labels:
 - `label:` defines a label at a location
 - `&label` is the location of a label, for ex.
 - `push &label goto` jumps to the location of `label`
 - `push &label load` basically loads a variable

Labels can be forward-referenced, e.g:
```assembly
push &main goto

// some code here
...

main:
   // main code goes here
   ...
```
Is valid

#### Variables
Variables can be easily created via the use of labels:
```assembly
push &counter load
1 +
push &counter store
// just incremented the value at counter

counter:
   0
```

## Tests

Two test/example programs are provided in `tests/`

### test_static.cpp
Implements the VM with a static program:
```c++
{
  OP_PUSH,1,
  OP_DUP,
  OP_PUSH,1,
  OP_ADD,
  OP_DUP,
  OP_PUSH,5,
  OP_EQ,
  OP_PUSH,2,
  OP_JZ,
  OP_HALT,
};
```
Which can be written as:
```assembly
push 1
loop:
  dup
  push 1 +
  dup
  push 5 =
  push &loop jz
halt
```
Which places incrementing numbers on the stack until it reaches 5

### test_open.cpp
Opens a compiled binary file and executes it in the VM
Usage: `test_open <filename>`

## Building the project and the examples
The supplied makefile should take care of everything, it also compiles the example assembly files in `asm/`
Note that it requires `python` and `cpp`!

## Planned:
 - higher-level language
 - easy-to-add custom instruction, like a syscall for nteracting with the host system
 - refactor
 - refactor
 - refactor
 - remove/make all debug switchable
 - add a way to modify the ram from the host system
 (make memory mapped io & stuff possible)
 
