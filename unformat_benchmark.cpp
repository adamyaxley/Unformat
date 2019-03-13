#include <benchmark/benchmark.h>
#include <regex>
#include <sstream>
#include <cstdio>
#include "unformat.h"

const char* g_input = "Harry is 18 years old and weighs 67.8 kilograms";
const std::string g_inputString = g_input;

#ifdef UNFORMAT_CPP17
#include <string_view>
using string_type = std::string_view;
#else
using string_type = std::string;
#endif

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

BENCHMARK_MAIN();