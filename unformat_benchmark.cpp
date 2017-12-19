#include <benchmark/benchmark.h>
#include <regex>
#include <sstream>
#include "unformat.h"

const std::string g_input = "Harry is 18 years old and weighs 67.8 kilograms";

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
		std::regex regex("([A-Za-z]+) is ([0-9]+) years old and weighs ([0-9\.]+) ([A-Za-z]+)");
		std::smatch matches;
		std::regex_search(g_input, matches, regex);

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

BENCHMARK_MAIN();