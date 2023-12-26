#include <iostream>
#include <cstring>

void Crash() {
	int* p = nullptr;
	*p = 1;
}

// The output is lost because there is a crash before the output is flushed.
// By default stdout buffers output before flushing it to the output file.
// Note: in Linux every IO device is presented as a file and the output file
// could be a network socket, pipe, or console.
void Stdout() {
	std::cout << "Log before crash";
	Crash();
}

// By default stderr flushes output immediately and output is not lost.
void Stderr() {
	std::cerr << "Log before crash";
	Crash();
}

// Output written to stdout gets flushed whenever there is a newline. Because
// of the newline, the output does not get lost.
void StdoutNewline() {
	std::cout << "Log before crash\n";
	Crash();
}

// The application can explicitly flush the output to prevent the output from
// being lost.
void StdoutFlush() {
	std::cout << "Log before crash" << std::flush;
	Crash();
}

// The stdout output buffer is flushed when it fills up. In this case the
// beginning of the message (which fills up the buffer) is not lost, but the
// end of the output which partially fills up a buffer is lost.
void StdoutLarge() {
	std::cout << "Log before crash ";
	for (int i = 0; i < 10000; i++) {
		std::cout << " " << i;
	}
	Crash();
}

void usage(const char **argv) {
	std::cerr << "Usage: " << argv[0] << " <example>\n";
	std::cerr << "Demonstration of logging messages before a crash. "
			  << "The examples include:\n";
	std::cerr << "  stdout - Log a message to standard output\n";
	std::cerr << "  stderr - Log a message to standard error\n";
	std::cerr << "  stdout-newline - Log a message with a newline to stdout\n";
	std::cerr << "  stdout-flush - Flush standard output after logging a message\n";
	std::cerr << "  stdout-large - Log a large message to standard output\n";
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
	} else if (!strcmp(argv[1], "stdout-newline")) {
		StdoutNewline();
	} else if (!strcmp(argv[1], "stdout-flush")) {
		StdoutFlush();
	} else if (!strcmp(argv[1], "stdout-large")) {
		StdoutLarge();
	} else {
		usage(argv);
	}
	return 0;
}
