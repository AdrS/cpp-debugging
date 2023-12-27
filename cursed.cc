#include <cassert>
#include <cstring>
#include <errno.h>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <string.h>
#include <sys/resource.h>

// Segfaults from dereferencing null pointers
////////////////////////////////////////////////////////////////////////////////
void DereferenceNullPointer() {
	int *ptr = nullptr;
	// Root cause: Dereferencing a null pointer
	int value = *ptr;
}

void WriteNullPointer() {
	int *ptr = nullptr;
	// Root cause: Dereferencing a null pointer
	*ptr = 123;
}

void CallNullFunctionPointer() {
	void (*f)() = nullptr;
	// Root cause: Dereferencing a null pointer
	(*f)();
}

// Fixes:
// 1. Check if the pointer is null before dereferencing it.
void NullPointerDereferenceFix1() {
	int *ptr = nullptr;
	if (ptr != nullptr) {
		*ptr = 123;
	}
}
// 2. Initialize the pointer with the address of an object.
void NullPointerDereferenceFix2() {
	int a = 123;
	int *ptr = &a;
	int value = *ptr;
}

// Segfaults from out-of-bounds array access
////////////////////////////////////////////////////////////////////////////////
void IndexOutOfBounds() {
	int a[100];
	// Root cause: array index in greater than the length of the array.
	int b = a[5000];
}

void IndexOutOfBoundsAssignment() {
	int a[100];
	// Root cause: array index in greater than the length of the array.
	a[5000] = 123;
}

// Fixes:
// 1. Check the array index is valid before array access.
void IndexOutOfBoundsFix1() {
	int a[100];
	int i = 5000;
	int b;
	if (i >= 0 && i < sizeof(a)/sizeof(int)) {
		b = a[i];
	} else {
		// Gracefully handle the invalid index without crashing the program
	}
}
// 2. Only assign valid values to index.
void IndexOutOfBoundsFix2() {
	int a[100];
	for (int i = 0; i < sizeof(a)/sizeof(int); i++) {
		int b = a[i];
	}

	// For other containers
	std::vector<int> v = {1, 2, 3, 4};
	for (int& x : v) {
		// Do something with element x
	}
}

// Segfaults from accessing invalid iterators
////////////////////////////////////////////////////////////////////////////////
void ResizeInvalidatesIterators() {
	std::vector<int> a = {1, 1};
	auto i1 = a.begin();
	auto i2 = i1 + 1;
	while(a.size() < 100000) {
		// Root cause: Adding elements to a vector can cause the vector to
		// resize to allocate space for the additional elements. After the
		// vector resizes, existing iterators become invalid.
		a.push_back(*i1 + *i2);
		i1++;
		i2++;
	}
}

void ResizeInvalidatesPointers() {
	std::vector<int> a = {1, 1};
	int *p1 = &a[0];
	int *p2 = &a[1];
	while(a.size() < 100000) {
		// Root cause: Adding elements to a vector can cause the vector to
		// resize to allocate space for the additional elements. When this
		// happens an pointers to elements become invalid.
		a.push_back(*p1 + *p2);
		p1++;
		p2++;
	}
}

// Fix:
// 1. TODO: pointers
// 2. TODO: lookup elements again

// Segfaults from accessing variables that went out of scope
////////////////////////////////////////////////////////////////////////////////
const int &ReturnReferenceToLocalVariableImpl() {
	int a = 7;
	// The compiler returns a warning
	// "warning: reference to local variable ‘a’ returned"
	return a;
}

void ReturnReferenceToLocalVariable() {
	const int& a = ReturnReferenceToLocalVariableImpl();
	// Root cause: The reference is to a variable allocated on the stack that
	// went out of scope when the function returned.
	int b = a;
}

int *ReturnPointerToLocalVariableImpl() {
	int a = 7;
	// The compiler returns a warning
	// "warning: address of local variable ‘a’ returned"
	return &a;
}

void ReturnPointerToLocalVariable() {
	int *a = ReturnPointerToLocalVariableImpl();
	// Root cause: The reference is to a variable allocated on the stack that
	// went out of scope when the function returned.
	int b = *a;
}

// Fix:
// 1. Return a copy of the variable.
int ReturnLocalVariableFix1Impl() {
	int a = 7;
	return a;
}
void ReturnLocalVariableFix1() {
	int a = ReturnLocalVariableFix1Impl();
}
// 2a. Allocate the variable on the heap.
int* ReturnLocalVariableFix2aImpl() {
	int *a = new int(7);
	return a;
}
void ReturnLocalVariableFix2a() {
	int *a = ReturnLocalVariableFix2aImpl();
	// Caller takes ownership of the memory
	delete a;
}

// 2b. Allocate the variable on the heap using a smart pointer.
std::unique_ptr<int> ReturnLocalVariableFix2bImpl() {
	auto a = std::make_unique<int>(7);
	return a;
}
void ReturnLocalVariableFix2b() {
	std::unique_ptr<int> a = ReturnLocalVariableFix2bImpl();
}

// TODO: This example does not crash, but still is incorrect.
class LambdaCaptureImpl {
public:
	LambdaCaptureImpl() {
		int a = 8;
		f_ = [&]() {
			// Root cause: The lambda captures a reference to a local variable
			// that goes out of scope before the lambda is called.
			 a = 123;
		};
	}

	void CallLambda() {
		f_();
	}

private:
	std::function<void()> f_;
};

void LambdaCapturesReferenceToLocalVariable() {
	LambdaCaptureImpl c;
	c.CallLambda();
}

// Fix:
// 1. Capture by value to make a copy.
// 2. Extend the lifetime of the variable by making it a member of the class
//    or allocating it on the heap.
// TODO:

// Invalid references

void UseAfterFree() {
	// TODO: malloc
	// free
	// use
}

// Stack overflow
////////////////////////////////////////////////////////////////////////////////

int FibMissingBaseCase(int n) {
	// Root cause: Recursive function is missing a base case. The function
	// recursively calls itself until all the stack space is exhausted and then
	// segfaults when passing the end of the stack.
	return FibMissingBaseCase(n - 1) + FibMissingBaseCase(n - 2);
}

void UnboundedRecursion() {
	int v = FibMissingBaseCase(10);
}

// Fix: Add a base case to terminate the recursion.
int Fib(int n) {
	if (n < 2) {
		return n;
	}
	return Fib(n - 1) + Fib(n - 2);
}

void UnboundedRecursionFix() {
	int v = Fib(10);
}

// Recursive function with a base case can overflow the stack if the recursion
// depth is deep enough.
struct Node {
	std::vector<std::unique_ptr<Node>> children;
};

void TraverseTree(const std::unique_ptr<Node>& root) {
	// Do something with the node
	for (const auto& child : root->children) {
		TraverseTree(child);
	}
}

void TooDeepRecursion() {
	std::unique_ptr<Node> root = std::make_unique<Node>();
	// Root cause: The max recursion depth is equal to the longest path in
	// the tree. For a large tree this is enough to overflow the call stack.
	for (int i = 0; i < 1'000'000; i++) {
		auto parent = std::make_unique<Node>();
		parent->children.emplace_back(std::move(root));
		root = std::move(parent);
	}
	TraverseTree(root);
}

// Fix: Allocate a stack on the heap to keep track of the problem state and
//      iterate thought the sub problems using a loop.
void TraverseTreeNoRecursion(const std::unique_ptr<Node>& root) {
	std::vector<const Node*> pending = {root.get()};
	while (!pending.empty()) {
		const Node* node = pending.back();
		pending.pop_back();
		// Do something with the node
		for (const auto& child : node->children) {
			pending.push_back(child.get());
		}
	}
}

// The object is deeply nested and the destructor calls would overflow the call
// stack. To get around this, this function traverses the tree and cuts the
// edges. Without the edges the destructor calls no longer generate deep call
// stacks.
void ClearTree(std::unique_ptr<Node> root) {
	std::vector<std::unique_ptr<Node>> pending;
	pending.emplace_back(std::move(root));
	while (!pending.empty()) {
		std::unique_ptr<Node> node = std::move(pending.back());
		pending.pop_back();
		for (auto& child : node->children) {
			pending.emplace_back(std::move(child));
		}
		node->children.clear();
	}
}

void TooDeepRecursionFix() {
	std::unique_ptr<Node> root = std::make_unique<Node>();
	for (int i = 0; i < 1'000'000; i++) {
		auto parent = std::make_unique<Node>();
		parent->children.emplace_back(std::move(root));
		root = std::move(parent);
	}
	TraverseTreeNoRecursion(root);
	ClearTree(std::move(root));
}

void LargeObjectOnStack() {
	// Root cause: The local variable is too large to fit on the stack.
	int a[10'000'000];
}

// Fixes:
// 1. Allocate large object on the heap.
void LargeObjectFix1() {
	int* a = new int[10'000'000];
	delete[] a;
}
// 2. Increase the stack size limit before calling the function.
void LargeObjectFix2() {
	struct rlimit limit;
	int error = getrlimit(RLIMIT_STACK, &limit);
	if (error != 0) {
		std::cerr << "Could not get stack size limit: " << strerror(errno);
		exit(1);
	}
	limit.rlim_cur += sizeof(int)*10'000'000;
	error = setrlimit(RLIMIT_STACK, &limit);
	if (error != 0) {
		std::cerr << "Could not increase stack size limit: " << strerror(errno);
		exit(1);
	}
	// No that the stack is larger, calling the function is ok.
	LargeObjectOnStack();
}

// Aborts
////////////////////////////////////////////////////////////////////////////////
void AssertFailure() {
	int a = 123;
	int upper_bound = 100;
	// Root cause: condition is false.
	assert(a < upper_bound);
}

// Fixes:
// 1) Fix the application logic to make the condition true.
// 2) Add logic to handle the error rather than aborting the program. Handling
// errors is generally preferable to crashing. For example a server processing
// requests from multiple clients may be able to continue processing requests
// even when one fails. Use assert for errors where it is impossible or
// unsafe to continue program execution. For example a server initialization
// can assert that critical dependencies like a database connection are
// successfully created. Also an assert crashing the program is preferable to
// data loss.
void AssertFailureFix() {
	int a = 123;
	int upper_bound = 100;
	if (a >= upper_bound) {
		// Error handling
		return;
	}
}

// OOM program
////////////////////////////////////////////////////////////////////////////////
void SetVirtualMemorySizeLimit(int size) {
	struct rlimit limit;
	limit.rlim_cur = size;
	limit.rlim_max = size;
	int error = setrlimit(RLIMIT_AS, &limit);
	if (error != 0) {
		std::cerr << "Could not set memory size limit: " << strerror(errno);
		exit(1);
	}
}

void OutOfMemory() {
	SetVirtualMemorySizeLimit(1<<22); // 4 MiB
	// Root cause: Allocating more memory than the resource limit.
	for (int i = 0; i < 1000; i++) {
		int *p = new int[1<<20];
	}
}

// Fixes:
// 1) Decrease the peak resource usage. In particular look for memory leaks or
//    or objects that can be deallocated sooner.
// 2) Increase the resource limit.
// 3) Catch the exception to avoid crashing.
void OutOfMemoryFix3() {
	SetVirtualMemorySizeLimit(1<<22); // 4 MiB
	for (int i = 0; i < 1000; i++) {
		try {
			int *p = new int[1<<20];
		} catch(std::bad_alloc& e) {
			std::cerr << "Bad alloc exception\n";
			break;
		}
	}
}
// 4) Use malloc
void OutOfMemoryFix4() {
	SetVirtualMemorySizeLimit(1<<22); // 4 MiB
	for (int i = 0; i < 1000; i++) {
		int *p = (int*)malloc(sizeof(int)*(1<<20));
		if (p == nullptr) {
			std::cerr << "Malloc error: " << strerror(errno) << "\n";
			break;
		}
	}
}

void CallUninitializedFunction() {
	// Root cause: function object has not been initialized.
	std::function<void()> f;
	f();
}


// Segfault
// - Use after free
// - Address not mapped
// - Writing to read only section
// - Trying to execute non executable section

// Invalid instruction
// - Call method on deallocated object
// - Call virtual method on deallocated object
//
// casting errors
// other signals

// Deadlock
// Race condition

struct Example {
	const char *name;
	const char *description;
	void (*run)();
	// TODO: Include list of fixes
};

struct ExampleGroup {
	const char *name;
	std::vector<Example> examples;
};

const std::vector<ExampleGroup> example_groups = {
	{
		.name = "Segfaults (Null Pointer)",
		.examples = {
			{
				.name = "dereference-null-pointer",
				.description = "Attempt to read a value from a null pointer",
				.run = DereferenceNullPointer,
			},
			{
				.name = "write-null-pointer",
				.description = "Attempt to assign a value to a null pointer",
				.run = WriteNullPointer,
			},
			{
				.name = "call-null-pointer",
				.description = "Attempt to call a null function pointer",
				.run = CallNullFunctionPointer,
			},
		},
	},
	{
		.name = "Segfaults",
		.examples = {
			{
				.name = "index-out-of-bounds",
				.description = "Access an array element past the end of the array",
				.run = IndexOutOfBounds,
			},
			{
				.name = "index-out-of-bounds-assignment",
				.description = "Attempt to assign a value to an array index past the end of the array",
				.run = IndexOutOfBoundsAssignment,
			},
			{
				.name = "resize-invalidates-iterators",
				.description = "Access vector elements through an iterator that is invalid because the vector was resized",
				.run = ResizeInvalidatesIterators,
			},
			{
				.name = "resize-invalidates-pointers",
				.description = "Access pointers to vector elements that are invalid after the vector is resized",
				.run = IndexOutOfBoundsAssignment,
			},
			{
				.name = "return-reference-to-local",
				.description = "Returns a reference to a local variable that goes out of scope",
				.run = ReturnReferenceToLocalVariable,
			},
			{
				.name = "return-pointer-to-local",
				.description = "Returns a pointer to a local variable that goes out of scope",
				.run = ReturnPointerToLocalVariable,
			},
			{
				.name = "lambda-capture-out-of-scope",
				.description = "Lambda captures variables that go out of scope",
				.run = LambdaCapturesReferenceToLocalVariable,
			},
		},
	},
	{
		.name = "Segfaults (Stack overflow)",
		.examples = {
			{
				.name = "unbounded-recursion",
				.description = "Overflow the stack with infinite recursion",
				.run = UnboundedRecursion,
			},
			{
				.name = "deep-recursion",
				.description = "Overflow the stack with deep recursion",
				.run = TooDeepRecursion,
			},
			{
				.name = "large-object-on-stack",
				.description = "Overflow the stack with a large local variable",
				.run = LargeObjectOnStack,
			},
		},
	},
	{
		.name = "Aborts",
		.examples = {
			{
				.name = "assert-failure",
				.description = "Program terminates due to false assert condition",
				.run = AssertFailure,
			},
			{
				.name = "oom",
				.description = "Program runs out of memory",
				.run = OutOfMemory,
			},
			{
				.name = "uninitialized-function",
				.description = "Call a function that has not been initialized",
				.run = CallUninitializedFunction,
			},
		},
	},
};

void usage(const char **argv) {
	std::cerr << "Usage: " << argv[0] << " <example name>\n\n";
	std::cerr << "Runs code with examples of common bugs.\n";
	for (const ExampleGroup& group : example_groups) {
		std::cerr << group.name << " Examples:\n";
		for (const Example& example : group.examples) {
			std::cerr << "  " << example.name << " - " << example.description << "\n";
		}
	}
	exit(1);
}


int main(const int argc, const char **argv) {
	if (argc != 2) {
		usage(argv);
	}
	const char *example_name = argv[1];
	for (const ExampleGroup& group : example_groups) {
		for (const Example& example : group.examples) {
			if (!strcmp(example.name, example_name)) {
				example.run();
				return 0;
			}
		}
	}
	usage(argv);
	return 0;
}
