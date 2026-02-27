#include <benchmark/benchmark.h>
#include <regex>
#include <sstream>
#include <cstdio>
#include "unformat.h"

const char* g_input = "Harry is 18 years old and weighs 67.8 kilograms";
const std::string g_inputString = g_input;

using string_type = ay::string_view;

static void Unformat(benchmark::State& state)
{
	string_type name;
	int age;
	float weight;
	string_type units;
	for (auto _ : state)
	{
		ay::unformat(g_input, "{} is {} years old and weighs {} {}", name, age, weight, units);
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(Unformat);

static void Unformat_ConstChar(benchmark::State& state)
{
	string_type name;
	int age;
	float weight;
	string_type units;
	for (auto _ : state)
	{
		const char* format = "{} is {} years old and weighs {} {}";
		ay::unformat(g_input, format, name, age, weight, units);
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(Unformat_ConstChar);

static void Unformat_ConstexprMakeFormat(benchmark::State& state)
{
	string_type name;
	int age;
	float weight;
	string_type units;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("{} is {} years old and weighs {} {}");
		ay::unformat(g_input, format, name, age, weight, units);
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(Unformat_ConstexprMakeFormat);

static void StdStringStream(benchmark::State& state)
{
	std::string name;
	int age;
	float weight;
	std::string units;
	for (auto _ : state)
	{
		std::istringstream stream(g_input);
		stream >> name;
		stream.ignore(2);
		stream >> age;
		stream.ignore(21);
		stream >> weight;
		stream >> units;
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(StdStringStream);

static void StdRegex(benchmark::State& state)
{
	std::string name;
	int age;
	float weight;
	std::string units;
	for (auto _ : state)
	{
		std::regex regex("([A-Za-z]+) is ([0-9]+) years old and weighs ([0-9\\.]+) ([A-Za-z]+)");
		std::smatch matches;
		std::regex_search(g_inputString, matches, regex);

		name = matches[1];
		age = std::stoi(matches[2]);
		weight = std::stof(matches[3]);
		units = matches[4];
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(StdRegex);

static void StdScanf(benchmark::State& state)
{
	char name[16];
	int age;
	float weight;
	char units[16];
	for (auto _ : state)
	{
		std::sscanf(g_input, "%s is %d years old and weighs %f %s", &name, &age, &weight, &units);
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(age);
		benchmark::DoNotOptimize(weight);
		benchmark::DoNotOptimize(units);
	}
}

BENCHMARK(StdScanf);

// --- Additional benchmarks for targeted optimization ---

// Single float extraction to isolate float parsing cost
static void Unformat_MakeFormat_SingleFloat(benchmark::State& state)
{
	float value;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("weight={}");
		ay::unformat("weight=67.8", format, value);
		benchmark::DoNotOptimize(value);
	}
}

BENCHMARK(Unformat_MakeFormat_SingleFloat);

// Single integer extraction with make_format
static void Unformat_MakeFormat_SingleInt(benchmark::State& state)
{
	int value;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("value={}");
		ay::unformat("value=42", format, value);
		benchmark::DoNotOptimize(value);
	}
}

BENCHMARK(Unformat_MakeFormat_SingleInt);

// Two integers with make_format
static void Unformat_MakeFormat_TwoInts(benchmark::State& state)
{
	int x, y;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("{},{}");
		ay::unformat("100,200", format, x, y);
		benchmark::DoNotOptimize(x);
		benchmark::DoNotOptimize(y);
	}
}

BENCHMARK(Unformat_MakeFormat_TwoInts);

// String extraction with make_format
static void Unformat_MakeFormat_StringOnly(benchmark::State& state)
{
	string_type name;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("hello {}!");
		ay::unformat("hello world!", format, name);
		benchmark::DoNotOptimize(name);
	}
}

BENCHMARK(Unformat_MakeFormat_StringOnly);

// Mixed types: string + int + float with make_format
static void Unformat_MakeFormat_MixedThree(benchmark::State& state)
{
	string_type name;
	int count;
	float price;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("{}: {} at {}");
		ay::unformat("apple: 5 at 1.99", format, name, count, price);
		benchmark::DoNotOptimize(name);
		benchmark::DoNotOptimize(count);
		benchmark::DoNotOptimize(price);
	}
}

BENCHMARK(Unformat_MakeFormat_MixedThree);

// Longer input string with make_format
static void Unformat_MakeFormat_LongInput(benchmark::State& state)
{
	string_type a, b, c;
	int x, y;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("{} connected to {} via {} with {} hops and {} ms latency");
		ay::unformat("server1 connected to server2 via gateway with 3 hops and 42 ms latency", format, a, b, c, x, y);
		benchmark::DoNotOptimize(a);
		benchmark::DoNotOptimize(b);
		benchmark::DoNotOptimize(c);
		benchmark::DoNotOptimize(x);
		benchmark::DoNotOptimize(y);
	}
}

BENCHMARK(Unformat_MakeFormat_LongInput);

// Compare: same workload without make_format (inline format string)
static void Unformat_Inline_LongInput(benchmark::State& state)
{
	string_type a, b, c;
	int x, y;
	for (auto _ : state)
	{
		ay::unformat("server1 connected to server2 via gateway with 3 hops and 42 ms latency",
			"{} connected to {} via {} with {} hops and {} ms latency", a, b, c, x, y);
		benchmark::DoNotOptimize(a);
		benchmark::DoNotOptimize(b);
		benchmark::DoNotOptimize(c);
		benchmark::DoNotOptimize(x);
		benchmark::DoNotOptimize(y);
	}
}

BENCHMARK(Unformat_Inline_LongInput);

// Integer-only parsing (4 ints) with make_format
static void Unformat_MakeFormat_FourInts(benchmark::State& state)
{
	int a, b, c, d;
	for (auto _ : state)
	{
		constexpr auto format = ay::make_format("{}.{}.{}.{}");
		ay::unformat("192.168.1.1", format, a, b, c, d);
		benchmark::DoNotOptimize(a);
		benchmark::DoNotOptimize(b);
		benchmark::DoNotOptimize(c);
		benchmark::DoNotOptimize(d);
	}
}

BENCHMARK(Unformat_MakeFormat_FourInts);

BENCHMARK_MAIN();