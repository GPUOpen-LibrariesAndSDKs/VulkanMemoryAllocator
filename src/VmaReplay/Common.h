#pragma once

#include "VmaUsage.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <array>
#include <type_traits>
#include <utility>
#include <chrono>
#include <string>

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

inline float ToFloatSeconds(duration d)
{
    return std::chrono::duration_cast<std::chrono::duration<float>>(d).count();
}

template <typename T>
inline T ceil_div(T x, T y)
{
    return (x+y-1) / y;
}

template <typename T>
static inline T align_up(T val, T align)
{
    return (val + align - 1) / align * align;
}

class CmdLineParser
{
public:
	enum RESULT
	{
		RESULT_OPT,
		RESULT_PARAMETER,
		RESULT_END,
		RESULT_ERROR,
	};

	CmdLineParser(int argc, char **argv);
	CmdLineParser(const char *CmdLine);
	
    void RegisterOpt(uint32_t Id, char Opt, bool Parameter);
	void RegisterOpt(uint32_t Id, const std::string &Opt, bool Parameter);
	
    RESULT ReadNext();
	uint32_t GetOptId();
	const std::string & GetParameter();

private:
	struct SHORT_OPT
	{
		uint32_t Id;
		char Opt;
		bool Parameter;

		SHORT_OPT(uint32_t Id, char Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	struct LONG_OPT
	{
		uint32_t Id;
		std::string Opt;
		bool Parameter;

		LONG_OPT(uint32_t Id, std::string Opt, bool Parameter) : Id(Id), Opt(Opt), Parameter(Parameter) { }
	};

	char **m_argv;
	const char *m_CmdLine;
	int m_argc;
	size_t m_CmdLineLength;
	size_t m_ArgIndex;

	bool ReadNextArg(std::string *OutArg);

	std::vector<SHORT_OPT> m_ShortOpts;
	std::vector<LONG_OPT> m_LongOpts;

	SHORT_OPT * FindShortOpt(char Opt);
	LONG_OPT * FindLongOpt(const std::string &Opt);

	bool m_InsideMultioption;
	std::string m_LastArg;
	size_t m_LastArgIndex;
	uint32_t m_LastOptId;
	std::string m_LastParameter;
};

/*
class RandomNumberGenerator
{
public:
    RandomNumberGenerator() : m_Value{GetTickCount()} {}
    RandomNumberGenerator(uint32_t seed) : m_Value{seed} { }
    void Seed(uint32_t seed) { m_Value = seed; }
    uint32_t Generate() { return GenerateFast() ^ (GenerateFast() >> 7); }

private:
    uint32_t m_Value;
    uint32_t GenerateFast() { return m_Value = (m_Value * 196314165 + 907633515); }
};

enum class CONSOLE_COLOR
{
    INFO,
    NORMAL,
    WARNING,
    ERROR_,
    COUNT
};

void SetConsoleColor(CONSOLE_COLOR color);

void PrintMessage(CONSOLE_COLOR color, const char* msg);
void PrintMessage(CONSOLE_COLOR color, const wchar_t* msg);

inline void Print(const char* msg) { PrintMessage(CONSOLE_COLOR::NORMAL, msg); }
inline void Print(const wchar_t* msg) { PrintMessage(CONSOLE_COLOR::NORMAL, msg); }
inline void PrintWarning(const char* msg) { PrintMessage(CONSOLE_COLOR::WARNING, msg); }
inline void PrintWarning(const wchar_t* msg) { PrintMessage(CONSOLE_COLOR::WARNING, msg); }
inline void PrintError(const char* msg) { PrintMessage(CONSOLE_COLOR::ERROR_, msg); }
inline void PrintError(const wchar_t* msg) { PrintMessage(CONSOLE_COLOR::ERROR_, msg); }

void PrintMessageV(CONSOLE_COLOR color, const char* format, va_list argList);
void PrintMessageV(CONSOLE_COLOR color, const wchar_t* format, va_list argList);
void PrintMessageF(CONSOLE_COLOR color, const char* format, ...);
void PrintMessageF(CONSOLE_COLOR color, const wchar_t* format, ...);
void PrintWarningF(const char* format, ...);
void PrintWarningF(const wchar_t* format, ...);
void PrintErrorF(const char* format, ...);
void PrintErrorF(const wchar_t* format, ...);
*/
