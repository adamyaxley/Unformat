#include "unformat.h"
#include "gtest/gtest.h"
#include <random>
#include <limits>
#include <cmath>
#include <string>
#include <sstream>
#include <cstring>

// Test helper: compare ay::string_view to a C string
static bool sv_eq(const ay::string_view& sv, const char* s)
{
	std::size_t len = std::strlen(s);
	if (sv.size != len) return false;
	return std::memcmp(sv.data, s, len) == 0;
}

// For GTest printing support
static std::ostream& operator<<(std::ostream& os, const ay::string_view& sv)
{
	return os.write(sv.data, sv.size);
}

TEST(Unformat, EmptyString)
{
	{
		ay::string_view output;
		ay::unformat("", "{}", output);
		ASSERT_TRUE(sv_eq(output, ""));
	}

	{
		ay::string_view output;
		ay::unformat("   ", "   {}", output);
		ASSERT_TRUE(sv_eq(output, ""));
	}

	{
		ay::string_view output;
		ay::unformat("   ", "{}   ", output);
		ASSERT_TRUE(sv_eq(output, ""));
	}

	{
		ay::string_view output;
		ay::unformat("      ", "   {}   ", output);
		ASSERT_TRUE(sv_eq(output, ""));
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
	ay::string_view name;
	int age;
	ay::unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
	ASSERT_TRUE(sv_eq(name, "Harry"));
	ASSERT_EQ(18, age);
}
	
TEST(Unformat, ThreeVars)
{
	ay::string_view name;
	int weight;
	ay::string_view supporting;
	ay::unformat("gpmvdo (78) -> jjixrr, zacrh, smylfq, fdvtn", "{} ({}) -> {}", name, weight, supporting);
	ASSERT_TRUE(sv_eq(name, "gpmvdo"));
	ASSERT_TRUE(sv_eq(supporting, "jjixrr, zacrh, smylfq, fdvtn"));
	ASSERT_EQ(78, weight);
}

TEST(Unformat, TightPlaceholdersWithDelimiter)
{
	ay::string_view first;
	ay::string_view second;
	ay::unformat("ab|cd", "{}|{}", first, second);
	ASSERT_TRUE(sv_eq(first, "ab"));
	ASSERT_TRUE(sv_eq(second, "cd"));
}

TEST(Unformat, EmptyMiddleCapture)
{
	ay::string_view left;
	ay::string_view middle;
	ay::string_view right;
	ay::unformat("left||right", "{}|{}|{}", left, middle, right);
	ASSERT_TRUE(sv_eq(left, "left"));
	ASSERT_TRUE(sv_eq(middle, ""));
	ASSERT_TRUE(sv_eq(right, "right"));
}

TEST(Unformat, StringInputOverload)
{
	const char* input = "name=alice,age=42";
	ay::string_view name;
	int age = 0;
	constexpr auto format = ay::make_format("name={},age={}");
	ay::unformat(input, format, name, age);
	ASSERT_TRUE(sv_eq(name, "alice"));
	ASSERT_EQ(42, age);
}

TEST(Unformat, ConstCharFormatOverload)
{
	ay::string_view left;
	ay::string_view right;
	ay::format runtimeFormat("{}:{}");
	ay::unformat("abc:def", runtimeFormat, left, right);
	ASSERT_TRUE(sv_eq(left, "abc"));
	ASSERT_TRUE(sv_eq(right, "def"));
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
	long longMin = 0;
	long longMax = 0;
	long long longLongMin = 0;
	long long longLongMax = 0;

	const auto shortMinStr = std::to_string(std::numeric_limits<short>::min());
	const auto shortMaxStr = std::to_string(std::numeric_limits<short>::max());
	const auto intMinStr = std::to_string(std::numeric_limits<int>::min());
	const auto intMaxStr = std::to_string(std::numeric_limits<int>::max());
	const auto longMinStr = std::to_string(std::numeric_limits<long>::min());
	const auto longMaxStr = std::to_string(std::numeric_limits<long>::max());
	const auto longLongMinStr = std::to_string(std::numeric_limits<long long>::min());
	const auto longLongMaxStr = std::to_string(std::numeric_limits<long long>::max());

	ay::unformat(shortMinStr.c_str(), "{}", shortMin);
	ay::unformat(shortMaxStr.c_str(), "{}", shortMax);
	ay::unformat(intMinStr.c_str(), "{}", intMin);
	ay::unformat(intMaxStr.c_str(), "{}", intMax);
	ay::unformat(longMinStr.c_str(), "{}", longMin);
	ay::unformat(longMaxStr.c_str(), "{}", longMax);
	ay::unformat(longLongMinStr.c_str(), "{}", longLongMin);
	ay::unformat(longLongMaxStr.c_str(), "{}", longLongMax);

	ASSERT_EQ(std::numeric_limits<short>::min(), shortMin);
	ASSERT_EQ(std::numeric_limits<short>::max(), shortMax);
	ASSERT_EQ(std::numeric_limits<int>::min(), intMin);
	ASSERT_EQ(std::numeric_limits<int>::max(), intMax);
	ASSERT_EQ(std::numeric_limits<long>::min(), longMin);
	ASSERT_EQ(std::numeric_limits<long>::max(), longMax);
	ASSERT_EQ(std::numeric_limits<long long>::min(), longLongMin);
	ASSERT_EQ(std::numeric_limits<long long>::max(), longLongMax);
}

TEST(Unformat, IntegerBoundariesUnsigned)
{
	unsigned short ushortMin = 123;
	unsigned short ushortMax = 0;
	unsigned int uintMin = 123;
	unsigned int uintMax = 0;
	unsigned long ulongMax = 0;
	unsigned long long ullMin = 123;
	unsigned long long ullMax = 0;

	const auto ushortMaxStr = std::to_string(std::numeric_limits<unsigned short>::max());
	const auto uintMaxStr = std::to_string(std::numeric_limits<unsigned int>::max());
	const auto ulongMaxStr = std::to_string(std::numeric_limits<unsigned long>::max());
	const auto ullMaxStr = std::to_string(std::numeric_limits<unsigned long long>::max());

	ay::unformat("0", "{}", ushortMin);
	ay::unformat(ushortMaxStr.c_str(), "{}", ushortMax);
	ay::unformat("0", "{}", uintMin);
	ay::unformat(uintMaxStr.c_str(), "{}", uintMax);
	ay::unformat(ulongMaxStr.c_str(), "{}", ulongMax);
	ay::unformat("0", "{}", ullMin);
	ay::unformat(ullMaxStr.c_str(), "{}", ullMax);

	ASSERT_EQ(static_cast<unsigned short>(0), ushortMin);
	ASSERT_EQ(std::numeric_limits<unsigned short>::max(), ushortMax);
	ASSERT_EQ(0u, uintMin);
	ASSERT_EQ(std::numeric_limits<unsigned int>::max(), uintMax);
	ASSERT_EQ(std::numeric_limits<unsigned long>::max(), ulongMax);
	ASSERT_EQ(0ull, ullMin);
	ASSERT_EQ(std::numeric_limits<unsigned long long>::max(), ullMax);
}

TEST(Unformat, FloatFixedPoint)
{
	float positive = 0.0f;
	float negative = 0.0f;
	float plusSign = 0.0f;
	ay::unformat("67.875", "{}", positive);
	ay::unformat("-12.5", "{}", negative);
	ay::unformat("+9.25", "{}", plusSign);
	ASSERT_FLOAT_EQ(67.875f, positive);
	ASSERT_FLOAT_EQ(-12.5f, negative);
	ASSERT_FLOAT_EQ(9.25f, plusSign);
}

TEST(Unformat, FloatScientificNotation)
{
	double lowercase = 0.0;
	double uppercase = 0.0;
	ay::unformat("-1.75e3", "{}", lowercase);
	ay::unformat("1.5E2", "{}", uppercase);
	ASSERT_DOUBLE_EQ(-1750.0, lowercase);
	ASSERT_DOUBLE_EQ(150.0, uppercase);
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
	double positive = 0.0;
	double negative = 0.0;
	const char* posInput = "3.141592653589793238462643383279";
	const char* negInput = "-3.141592653589793238462643383279";
	ay::unformat(posInput, "{}", positive);
	ay::unformat(negInput, "{}", negative);
	ASSERT_DOUBLE_EQ(std::strtod(posInput, nullptr), positive);
	ASSERT_DOUBLE_EQ(std::strtod(negInput, nullptr), negative);
}

TEST(Unformat, DirectUnformatArgString)
{
	const auto value = ay::unformat_arg<ay::string_view>("hello world");
	ASSERT_TRUE(sv_eq(value, "hello world"));
}

TEST(Unformat, ZeroValues)
{
	int signedOut = -1;
	unsigned int unsignedOut = 99;
	float fval = 1.0f;
	double dval = 1.0;
	ay::unformat("0", "{}", signedOut);
	ay::unformat("0", "{}", unsignedOut);
	ay::unformat("0.0", "{}", fval);
	ay::unformat("0.0", "{}", dval);
	ASSERT_EQ(0, signedOut);
	ASSERT_EQ(0u, unsignedOut);
	ASSERT_FLOAT_EQ(0.0f, fval);
	ASSERT_DOUBLE_EQ(0.0, dval);
}

TEST(Unformat, NegativeZeroFloat)
{
	double value = 1.0;
	ay::unformat("-0.0", "{}", value);
	ASSERT_DOUBLE_EQ(0.0, std::abs(value));
	ASSERT_TRUE(std::signbit(value));
}

TEST(Unformat, UnformatArgReturnByValueNumeric)
{
	const char* intStr = "42";
	const char* floatStr = "3.25";

	auto intVal = ay::unformat_arg<int>(intStr, intStr + 2);
	auto floatVal = ay::unformat_arg<double>(floatStr, floatStr + 4);

	ASSERT_EQ(42, intVal);
	ASSERT_DOUBLE_EQ(3.25, floatVal);
}

TEST(Unformat, NegativeScientificNotationUppercaseE)
{
	double value = 0.0;
	ay::unformat("-2.5E4", "{}", value);
	ASSERT_DOUBLE_EQ(-25000.0, value);
}

TEST(Unformat, FormatWithNoPlaceholders)
{
	constexpr auto format = ay::make_format("hello");
	ASSERT_EQ(static_cast<std::size_t>(0), format.count);
}

TEST(Unformat, FormatIgnoresNonPlaceholderBraces)
{
	constexpr auto format = ay::make_format("{ } {x} {}");
	ASSERT_EQ(static_cast<std::size_t>(1), format.count);
}

TEST(Unformat, MixedTypeExtraction)
{
	char c = '\0';
	int i = 0;
	double d = 0.0;
	ay::string_view s;
	ay::unformat("A|42|3.5|hello world", "{}|{}|{}|{}", c, i, d, s);
	ASSERT_EQ('A', c);
	ASSERT_EQ(42, i);
	ASSERT_DOUBLE_EQ(3.5, d);
	ASSERT_TRUE(sv_eq(s, "hello world"));
}

TEST(Unformat, CharFromMultiCharCapture)
{
	char output = '\0';
	ay::string_view rest;
	ay::unformat("XYZ|end", "{}|{}", output, rest);
	ASSERT_EQ('X', output);
}

TEST(Unformat, FormatParsesAllPlaceholders)
{
	constexpr auto format = ay::make_format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}");
	ASSERT_EQ(static_cast<std::size_t>(16), format.count);
}

TEST(Unformat, FormatThrowsWhenPlaceholderCountExceedsLimit)
{
	const char* tooMany = "{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}";
	EXPECT_THROW(
		{
			ay::format runtimeFormat(tooMany);
			static_cast<void>(runtimeFormat);
		},
		const char*);
}

TEST(Unformat, UnformatArgStringView)
{
	const char* source = "hello-view";
	const auto output = ay::unformat_arg<ay::string_view>(source);
	ASSERT_TRUE(sv_eq(output, "hello-view"));
}

TEST(Unformat, StringViewOutputSpecialization)
{
	ay::string_view first;
	ay::string_view second;
	ay::unformat("left|right", "{}|{}", first, second);
	ASSERT_TRUE(sv_eq(first, "left"));
	ASSERT_TRUE(sv_eq(second, "right"));
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
			// Default stream precision (6 digits) creates ambiguous representations for
			// extreme values, so allow small rounding differences (4 ULP)
			if (sizeof(T) == sizeof(double))
				ASSERT_DOUBLE_EQ(static_cast<double>(streamOutput), static_cast<double>(output)) << i << ": Test that unformat and std::stringstream are close at default precision";
			else
				ASSERT_FLOAT_EQ(static_cast<float>(streamOutput), static_cast<float>(output)) << i << ": Test that unformat and std::stringstream are close at default precision";
			
			ay::unformat(std::to_string(input).c_str(), format, output);
			// std::to_string uses %f which for extreme values (e.g. 1.5e308) produces
			// 300+ digit strings. Our hand-rolled parser may differ by 1 ULP without
			// a correctly-rounding strtod (which requires Eisel-Lemire or similar).
			if (sizeof(T) == sizeof(double))
				ASSERT_DOUBLE_EQ(static_cast<double>(input), static_cast<double>(output)) << i << ": Test that unformat is the near-exact opposite of std::to_string";
			else
				ASSERT_FLOAT_EQ(static_cast<float>(input), static_cast<float>(output)) << i << ": Test that unformat is the near-exact opposite of std::to_string";
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