#include <iostream>
#include <cstring>

void Crash() {
	int* p = nullptr;
	*p = 1;
}

void Stdout() {
	std::cout << "Log before crash";
	Crash();
}

void Stderr() {
	std::cerr << "Log before crash";
	Crash();
}

void StdoutFlush() {
	std::cout << "Log before crash" << std::flush;
	Crash();
}

//struct LoggingExample {
//	const char* name;
//	const char* description;
//	void (*f)();
//};
//
//const LoggingExample[] examples = {
//	{
//		.name = "stdout",
//		.description = "Log message to standard output",
//		.f = Stdout
//	},
//	{
//		.name = "stderr",
//		.description = "Log message to standard error",
//		.f = Stderr
//	},
//	{
//		.name = "stdout-flush",
//		.description = "Flush stdout after logging messages",
//		.f = StdoutFlush
//	},
//};

void usage(const char **argv) {
	std::cerr << "Usage: " << argv[0] << " <example>\n";
	std::cerr << "Demonstration of logging messages before a crash. "
			  << "The examples inlucde:\n";
	std::cerr << "  stdout - Log messages to standard output\n";
	std::cerr << "  stderr - Log messages to standard error\n";
	std::cerr << "  stdout-flush - Flush standard output after logging a message\n";
	exit(1);
}

int main(const int argc, const char **argv) {
	if (argc != 2) {
		usage(argv);
	}
	if (!strcmp(argv[1], "stdout")) {
		Stdout();
	} else if (!strcmp(argv[1], "stderr")) {
		Stderr();
	} else if (!strcmp(argv[1], "stdout-flush")) {
		StdoutFlush();
	} else {
		usage(argv);
	}
	return 0;
}
