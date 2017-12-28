#include "unformat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <random>
#include <limits>

TEST(Unformat, Basic)
{
	// Basic test to extract a single integer
	int output;
	ay::unformat("4", "{}", output);
	ASSERT_EQ(4, output);

	ay::unformat("  5  ", "  {}  ", output);
	ASSERT_EQ(5, output);

	ay::unformat("  6", "  {}", output);
	ASSERT_EQ(6, output);

	ay::unformat("7  ", "{}  ", output);
	ASSERT_EQ(7, output);
}

TEST(Unformat, TwoVars)
{
	std::string name;
	int age;
	ay::unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
	ASSERT_EQ(std::string("Harry"), name);
	ASSERT_EQ(18, age);
}
	
TEST(Unformat, ThreeVars)
{
	std::string name;
	int weight;
	std::string supporting;
	ay::unformat("gpmvdo (78) -> jjixrr, zacrh, smylfq, fdvtn", "{} ({}) -> {}", name, weight, supporting);
	ASSERT_EQ(std::string("gpmvdo"), name);
	ASSERT_EQ(std::string("jjixrr, zacrh, smylfq, fdvtn"), supporting);
	ASSERT_EQ(78, weight);
}

namespace
{
	constexpr static auto FUZZ_COUNT = 10000;

	template <typename T>
	void fuzzReal()
	{
		fuzz<T, std::uniform_real_distribution<T>>();
	}

	template <typename T>
	void fuzzInt()
	{
		fuzz<T, std::uniform_int_distribution<T>>();
	}

	template <typename T, typename DISTRIBUTION>
	void fuzz()
	{
		std::mt19937 engine;
		DISTRIBUTION dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
		constexpr auto format = ay::make_format("{}");

		for (int i = 0; i < FUZZ_COUNT; i++)
		{
			const auto input = dist(engine);
			T output;

			std::stringstream stream;
			stream << input;
			ay::unformat(stream.str().c_str(), format, output);
			T streamOutput;
			stream >> streamOutput;
			ASSERT_EQ(streamOutput, output) << "Test that unformat and std::stringstream are equal at default precision";
			
			ay::unformat(std::to_string(input).c_str(), format, output);
			ASSERT_EQ(input, output) << "Test that unformat is the exact lossless opposite of std::to_string";
		}
	}
}

TEST(Unformat, FuzzFloat)
{
	fuzzReal<float>();
}

TEST(Unformat, FuzzDouble)
{
	fuzzReal<double>();
}

TEST(Unformat, FuzzShort)
{
	fuzzInt<short>();
}

TEST(Unformat, FuzzUnsignedShort)
{
	fuzzInt<unsigned short>();
}

TEST(Unformat, FuzzInt)
{
	fuzzInt<int>();
}

TEST(Unformat, FuzzUnsignedInt)
{
	fuzzInt<unsigned int>();
}

TEST(Unformat, FuzzLong)
{
	fuzzInt<long>();
}

TEST(Unformat, FuzzUnsignedLong)
{
	fuzzInt<unsigned long>();
}

TEST(Unformat, FuzzLongLong)
{
	fuzzInt<long long>();
}

TEST(Unformat, FuzzUnsignedLongLong)
{
	fuzzInt<unsigned long long>();
}