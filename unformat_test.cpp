#include "unformat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

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
