# C++ Debugging

Examples of common C++ errors and how to debug and fix them. This repository
includes source code for a "cursed" binary with examples of common C++ coding
errors. The comments and README explain each bug and give tips on root causing
and fixing each bug.

## Directions

Clone the repository

```
git clone https://github.com/adrs/cpp-debugging.git
```

### Logging Demo

This demo shows how log messages are lost during a crash if they are not
flushed.

```
$ make logging
g++ -g logging.cc -o logging
$ ./logging
Usage: ./logging <example>
Demonstration of logging messages before a crash. The examples include:
  stdout - Log a message to standard output
  stderr - Log a message to standard error
  stdout-newline - Log a message with a newline to stdout
  stdout-flush - Flush standard output after logging a message
  stdout-large - Log a large message to standard output
$ ./logging stdout
Segmentation fault (core dumped)
$ ./logging stderr
Log before crashSegmentation fault (core dumped)
$ ./logging stdout-newline
Log before crash
Segmentation fault (core dumped)
$ ./logging stdout-flush
Log before crashSegmentation fault (core dumped)
$ ./logging stdout-large
Log before crash  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 ...
9835 9836 9837 9838 9839 9840 9841 9842 9843 Segmentation fault (core dumped)
```

Messages logged to standard output are lost if the program crashes before they
are flushed. Standard output is flushed when the output contains a newline, the
output buffer fills up, or the application calls flush. In the first example, a
small message without a newline is logged to stdout before the crash and the
output is lost. In the last example a large message containing 0 1 2, ... 9999
is logged to stdout. The beginning of the message gets flushed because it
filled up the output buffer. The end of the message partially refilled the
output buffer and is lost because the partially full buffer is not flushed
before the crash.

### Cursed Binary Demo

This demo shows examples of common errors causing programs to crash. Look at
the comments in `cursed.cc` for descriptions about the root cause of each
error and fixes.

Compile the binary. Notice some of the examples trigger compiler warnings.
```
$ make cursed
g++ -g cursed.cc -o cursed
cursed.cc: In function ‘const int& ReturnReferenceToLocalVariableImpl()’:
cursed.cc:126:9: warning: reference to local variable ‘a’ returned [-Wreturn-local-addr]
  126 |  return a;
      |         ^
cursed.cc:123:6: note: declared here
  123 |  int a = 7;
      |      ^
cursed.cc: In function ‘int* ReturnPointerToLocalVariableImpl()’:
cursed.cc:140:9: warning: address of local variable ‘a’ returned [-Wreturn-local-addr]
  140 |  return &a;
      |         ^~
cursed.cc:137:6: note: declared here
  137 |  int a = 7;
      |      ^
```


Run the `cursed` binary without arguments to see a list of the examples.

```
$ ./cursed
Usage: ./cursed <example name>

Runs code with examples of common bugs.
Segfaults (Null Pointer) Examples:
  dereference-null-pointer - Attempt to read a value from a null pointer
  write-null-pointer - Attempt to assign a value to a null pointer
  call-null-pointer - Attempt to call a null function pointer
Segfaults Examples:
  index-out-of-bounds - Access an array element past the end of the array
  index-out-of-bounds-assignment - Attempt to assign a value to an array index past the end of the array
  resize-invalidates-iterators - Access vector elements through an iterator that is invalid because the vector was resized
  resize-invalidates-pointers - Access pointers to vector elements that are invalid after the vector is resized
  return-reference-to-local - Returns a reference to a local variable that goes out of scope
  return-pointer-to-local - Returns a pointer to a local variable that goes out of scope
  lambda-capture-out-of-scope - Lambda captures variables that go out of scope
Segfaults (Stack overflow) Examples:
  unbounded-recursion - Overflow the stack with infinite recursion
  deep-recursion - Overflow the stack with deep recursion
  large-object-on-stack - Overflow the stack with a large local variable
Aborts Examples:
  assert-failure - Program terminates due to false assert condition
  oom - Program runs out of memory
```

Run the `cursed` binary with the name of an example to see how the program
crashes.

```
$ ./cursed dereference-null-pointer
Segmentation fault (core dumped)
$ echo $?
139
```

Run an example in a debugger to see more details.

```
$ gdb -q --ex=run --args ./cursed dereference-null-pointer
Reading symbols from ./cursed...
Starting program: /home/dev/Documents/cpp-debugging/cursed dereference-null-pointer

Program received signal SIGSEGV, Segmentation fault.
0x000055555555543d in DereferenceNullPointer () at cursed.cc:16
16              int value = *ptr;
(gdb) p ptr
$1 = (int *) 0x0
```
