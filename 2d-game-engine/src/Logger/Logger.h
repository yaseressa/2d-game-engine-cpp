#pragma once
#include <iostream>
#include <ctime>
#include <cstring>
#include <vector>
using namespace std;
struct LogEntryRecord {
	int type;
	string message;
};
class Logger
{
private:
	static void SetConsoleColor(const string& colorCode) {
		std::cout << "\033[" << colorCode << "m";
	};
	static vector<LogEntryRecord> Log;
	static void WriteToConsole(string type, const string& message, const string& colorCode) {
		time_t now = time(nullptr);
		tm localTime;
		localtime_s(&localTime, &now);
		char TimeString[26];
		asctime_s(TimeString, sizeof TimeString, &localTime);
		string timeFormatted = "[" + string(TimeString).substr(0, 24) + "]";
		SetConsoleColor(colorCode);
		char* typeChar = type.data();
		_strupr_s(typeChar, sizeof typeChar);
		LogEntryRecord logEntryRc{ atoi(type.data()), timeFormatted + " -> " + string(typeChar) + ": " + message };
		cout << logEntryRc.message << endl;
	};

public:
	static void Info(const string& message);
	static void Error(const string& message);
	static void Warn(const string& message); 
	static void Debug(const string& message);
	static void Fatal(const string& message);
};