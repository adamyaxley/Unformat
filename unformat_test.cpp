#include "unformat.h"
#include <cassert>

int main()
{
	// Basic test to extract a single integer
	{
		int output;
		ay::unformat("4", "{}", output);
		assert(4 == output);

		ay::unformat("  5  ", "  {}  ", output);
		assert(5 == output);

		ay::unformat("  6", "  {}", output);
		assert(6 == output);

		ay::unformat("7  ", "{}  ", output);
		assert(7 == output);
	}

	// Simple example extracting a string and an integer
	{
		std::string name;
		int age;
		ay::unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
		assert(std::string("Harry") == name);
		assert(18 == age);
	}
	
	// Slightly more complicated example
	{
		std::string name;
		int weight;
		std::string supporting;
		ay::unformat("gpmvdo (78) -> jjixrr, zacrh, smylfq, fdvtn", "{} ({}) -> {}", name, weight, supporting);
		assert(std::string("gpmvdo") == name);
		assert(std::string("jjixrr, zacrh, smylfq, fdvtn") == supporting);
		assert(78 == weight);
	}
}
