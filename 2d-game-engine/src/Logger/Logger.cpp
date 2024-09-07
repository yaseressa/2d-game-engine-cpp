#include "Logger.h"
using namespace std;

vector<LogEntryRecord> Logger::Log;

void Logger::Info(const string& message) {
	WriteToConsole("info", message, "32");
}

void Logger::Error(const string& message) {
	WriteToConsole("error", message, "31");
}

void Logger::Warn(const string& message) {
	WriteToConsole("warn", message, "33");
}

void Logger::Debug(const string& message) {
	WriteToConsole("debug", message, "34");
}

void Logger::Fatal(const string& message) {
	WriteToConsole("fatal", message, "35");
}
