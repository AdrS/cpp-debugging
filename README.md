# C++ Debugging Guide

Examples of common C++ errors and how to debug and fix them. This repository
includes source code for a "cursed" binary with examples of common C++ coding
errors. The comments and README explain each bug and give tips on root causing
and fixing each bug.

## Directions

Clone the repository

```
git clone TODO
```

Compile and run on a Linux instance
```
$ make
g++ -g cursed_main.cc -o cursed_binary
```

By default the cursed binary prints a list of the examples to try.

```
$ ./cursed_binary
Usage: ./cursed_binary <example name>

Runs code with examples of common bugs.
Null Pointer Examples:
  dereference-null-pointer - Attempt to read a value from a null pointer
  write-null-pointer - Attempt to assign a value to a null pointer
  call-null-pointer - Attempt to call a null function pointer
  ...
```

Pass the name of an example to run

```
$ ./cursed_binary dereference-null-pointer
Segmentation fault (core dumped)
$ echo $?
139
```

Run an example in a debugger

```
$ gdb -q --ex=run --args ./cursed_binary dereference-null-pointer
Reading symbols from ./cursed_binary...
Starting program: /home/dev/Documents/cpp-debugging-examples/cursed_binary dereference-null-pointer

Program received signal SIGSEGV, Segmentation fault.
0x000055555555531d in DereferenceNullPointer () at cursed_main.cc:9
9               int value = *ptr;
(gdb) p ptr
$1 = (int *) 0x0
...
```

```
Program received signal SIGSEGV, Segmentation fault.
```
