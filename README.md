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
the comments in `cursed_main.cc` for descriptions about the root cause of each
error and fixes.

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

Pass the name of an example to run.

```
$ ./cursed_binary dereference-null-pointer
Segmentation fault (core dumped)
$ echo $?
139
```

Run an example in a debugger to see more details.

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
