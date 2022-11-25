
#include "util.h"

#include <stdexcept>
#include <sstream>

void ThrowNullPointer(const std::string& file, const std::string& function, int line) {
	const auto n = file.find_last_of("/\\");

	std::string shortFile{};
	if (n == std::string::npos) shortFile = file; else shortFile = file.substr(n + 1);

	std::stringstream ss{};
	ss << "A null pointer was passed to a function.\n\nFunciton: " << function << "\nFile: " << shortFile << "\nline: " << line;
	throw(std::invalid_argument(ss.str().c_str()));
}

void ThrowPrettyException(const std::string& file, const std::string& function, int line, const std::string& message) {
	const auto n = file.find_last_of("/\\");

	std::string shortFile{};
	if (n == std::string::npos) shortFile = file; else shortFile = file.substr(n + 1);

	std::stringstream ss{};
	ss << message << "\n\nFunciton: " << function << "\nFile: " << shortFile << "\nline: " << line;
	throw(std::exception(ss.str().c_str()));
}