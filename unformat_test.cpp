#include "unformat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <random>
#include <limits>
#include <cmath>

TEST(Unformat, EmptyString)
{
	{
		std::string output("NotEmpty");
		ay::unformat("", "{}", output);
		ASSERT_EQ("", output);
	}

	{
		std::string output("NotEmpty");
		ay::unformat("   ", "   {}", output);
		ASSERT_EQ("", output);
	}

	{
		std::string output("NotEmpty");
		ay::unformat("   ", "{}   ", output);
		ASSERT_EQ("", output);
	}

	{
		std::string output("NotEmpty");
		ay::unformat("      ", "   {}   ", output);
		ASSERT_EQ("", output);
	}
}

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

TEST(Unformat, TightPlaceholdersWithDelimiter)
{
	std::string first;
	std::string second;
	ay::unformat("ab|cd", "{}|{}", first, second);
	ASSERT_EQ(std::string("ab"), first);
	ASSERT_EQ(std::string("cd"), second);
}

TEST(Unformat, EmptyMiddleCapture)
{
	std::string left;
	std::string middle;
	std::string right;
	ay::unformat("left||right", "{}|{}|{}", left, middle, right);
	ASSERT_EQ(std::string("left"), left);
	ASSERT_EQ(std::string(""), middle);
	ASSERT_EQ(std::string("right"), right);
}

TEST(Unformat, StringInputOverload)
{
	std::string input("name=alice,age=42");
	std::string name;
	int age = 0;
	constexpr auto format = ay::make_format("name={},age={}");
	ay::unformat(input, format, name, age);
	ASSERT_EQ(std::string("alice"), name);
	ASSERT_EQ(42, age);
}

TEST(Unformat, ConstCharFormatOverload)
{
	std::string left;
	std::string right;
	ay::format runtimeFormat("{}:{}");
	ay::unformat("abc:def", runtimeFormat, left, right);
	ASSERT_EQ(std::string("abc"), left);
	ASSERT_EQ(std::string("def"), right);
}

TEST(Unformat, CharSpecialization)
{
	char output = '\0';
	ay::unformat("Q", "{}", output);
	ASSERT_EQ('Q', output);
}

TEST(Unformat, UnsignedCharSpecialization)
{
	unsigned char output = 0;
	ay::unformat("R", "{}", output);
	ASSERT_EQ(static_cast<unsigned char>('R'), output);
}

TEST(Unformat, SignedIntegerSigns)
{
	int negative = 0;
	int positive = 0;
	short withLeadingZero = 0;
	ay::unformat("-12", "{}", negative);
	ay::unformat("+19", "{}", positive);
	ay::unformat("007", "{}", withLeadingZero);
	ASSERT_EQ(-12, negative);
	ASSERT_EQ(19, positive);
	ASSERT_EQ(7, withLeadingZero);
}

TEST(Unformat, UnsignedIntegerLeadingZeros)
{
	unsigned int value = 0;
	ay::unformat("000123", "{}", value);
	ASSERT_EQ(123u, value);
}

TEST(Unformat, IntegerBoundariesSigned)
{
	short shortMin = 0;
	short shortMax = 0;
	int intMin = 0;
	int intMax = 0;
	long long longLongMin = 0;
	long long longLongMax = 0;

	const auto shortMinStr = std::to_string(std::numeric_limits<short>::min());
	const auto shortMaxStr = std::to_string(std::numeric_limits<short>::max());
	const auto intMinStr = std::to_string(std::numeric_limits<int>::min());
	const auto intMaxStr = std::to_string(std::numeric_limits<int>::max());
	const auto longLongMinStr = std::to_string(std::numeric_limits<long long>::min());
	const auto longLongMaxStr = std::to_string(std::numeric_limits<long long>::max());

	ay::unformat(shortMinStr.c_str(), "{}", shortMin);
	ay::unformat(shortMaxStr.c_str(), "{}", shortMax);
	ay::unformat(intMinStr.c_str(), "{}", intMin);
	ay::unformat(intMaxStr.c_str(), "{}", intMax);
	ay::unformat(longLongMinStr.c_str(), "{}", longLongMin);
	ay::unformat(longLongMaxStr.c_str(), "{}", longLongMax);

	ASSERT_EQ(std::numeric_limits<short>::min(), shortMin);
	ASSERT_EQ(std::numeric_limits<short>::max(), shortMax);
	ASSERT_EQ(std::numeric_limits<int>::min(), intMin);
	ASSERT_EQ(std::numeric_limits<int>::max(), intMax);
	ASSERT_EQ(std::numeric_limits<long long>::min(), longLongMin);
	ASSERT_EQ(std::numeric_limits<long long>::max(), longLongMax);
}

TEST(Unformat, IntegerBoundariesUnsigned)
{
	unsigned short ushortMin = 123;
	unsigned short ushortMax = 0;
	unsigned int uintMin = 123;
	unsigned int uintMax = 0;
	unsigned long long ullMin = 123;
	unsigned long long ullMax = 0;

	const auto ushortMaxStr = std::to_string(std::numeric_limits<unsigned short>::max());
	const auto uintMaxStr = std::to_string(std::numeric_limits<unsigned int>::max());
	const auto ullMaxStr = std::to_string(std::numeric_limits<unsigned long long>::max());

	ay::unformat("0", "{}", ushortMin);
	ay::unformat(ushortMaxStr.c_str(), "{}", ushortMax);
	ay::unformat("0", "{}", uintMin);
	ay::unformat(uintMaxStr.c_str(), "{}", uintMax);
	ay::unformat("0", "{}", ullMin);
	ay::unformat(ullMaxStr.c_str(), "{}", ullMax);

	ASSERT_EQ(static_cast<unsigned short>(0), ushortMin);
	ASSERT_EQ(std::numeric_limits<unsigned short>::max(), ushortMax);
	ASSERT_EQ(0u, uintMin);
	ASSERT_EQ(std::numeric_limits<unsigned int>::max(), uintMax);
	ASSERT_EQ(0ull, ullMin);
	ASSERT_EQ(std::numeric_limits<unsigned long long>::max(), ullMax);
}

TEST(Unformat, FloatFixedPoint)
{
	float value = 0.0f;
	ay::unformat("67.875", "{}", value);
	ASSERT_FLOAT_EQ(67.875f, value);
}

TEST(Unformat, FloatNegativeFixedPoint)
{
	float value = 0.0f;
	ay::unformat("-12.5", "{}", value);
	ASSERT_FLOAT_EQ(-12.5f, value);
}

TEST(Unformat, FloatWithPlusSign)
{
	float value = 0.0f;
	ay::unformat("+9.25", "{}", value);
	ASSERT_FLOAT_EQ(9.25f, value);
}

TEST(Unformat, FloatScientificNotation)
{
	double value = 0.0;
	ay::unformat("-1.75e3", "{}", value);
	ASSERT_DOUBLE_EQ(-1750.0, value);
}

TEST(Unformat, FloatScientificNotationUppercaseE)
{
	double value = 0.0;
	ay::unformat("1.5E2", "{}", value);
	ASSERT_DOUBLE_EQ(150.0, value);
}

TEST(Unformat, FloatScientificNotationSignedExponent)
{
	double up = 0.0;
	double down = 0.0;
	ay::unformat("1e+3", "{}", up);
	ay::unformat("1e-3", "{}", down);
	ASSERT_DOUBLE_EQ(1000.0, up);
	ASSERT_DOUBLE_EQ(0.001, down);
}

TEST(Unformat, FloatWithoutLeadingOrTrailingIntegerDigits)
{
	double leadingMissing = 0.0;
	double trailingMissing = 0.0;
	ay::unformat(".5", "{}", leadingMissing);
	ay::unformat("5.", "{}", trailingMissing);
	ASSERT_DOUBLE_EQ(0.5, leadingMissing);
	ASSERT_DOUBLE_EQ(5.0, trailingMissing);
}

TEST(Unformat, FloatLongInputFallback)
{
	double value = 0.0;
	const char* input = "3.141592653589793238462643383279";
	ay::unformat(input, "{}", value);
	ASSERT_DOUBLE_EQ(std::strtod(input, nullptr), value);
}

TEST(Unformat, DirectUnformatArgString)
{
	const auto value = ay::unformat_arg<std::string>("hello world");
	ASSERT_EQ(std::string("hello world"), value);
}


TEST(Unformat, FormatParsesAllPlaceholders)
{
	constexpr auto format = ay::make_format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}");
	ASSERT_EQ(static_cast<std::size_t>(16), format.count);
}

TEST(Unformat, FormatThrowsWhenPlaceholderCountExceedsLimit)
{
	const std::string tooMany = "{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}";
	EXPECT_THROW(
		{
			ay::format runtimeFormat(tooMany.c_str());
			static_cast<void>(runtimeFormat);
		},
		std::exception);
}

#ifdef UNFORMAT_CPP17
TEST(Unformat, UnformatArgStringView)
{
	std::string source = "hello-view";
	std::string_view view(source.data(), source.size());
	const auto output = ay::unformat_arg<std::string_view>(view);
	ASSERT_EQ(view, output);
}

TEST(Unformat, StringViewOutputSpecialization)
{
	std::string source = "left|right";
	std::string_view first;
	std::string_view second;
	ay::unformat(source.c_str(), "{}|{}", first, second);
	ASSERT_EQ(std::string_view("left"), first);
	ASSERT_EQ(std::string_view("right"), second);
}
#endif

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
			ASSERT_EQ(streamOutput, output) << i << ": Test that unformat and std::stringstream are equal at default precision";
			
			ay::unformat(std::to_string(input).c_str(), format, output);
			ASSERT_EQ(input, output) << i << ": Test that unformat is the exact lossless opposite of std::to_string";
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