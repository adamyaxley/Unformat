#pragma once

#include <type_traits>
#include <string>
#include <sstream>

namespace ay
{
	// Unformat a single argument using std::istringstream
	template <typename T>
	void unformat_arg(const char* input, std::size_t size, T& output)
	{
		std::string value(input, size);
		std::istringstream stream(value);
		stream >> output;
	}

	// This is an optimisation to remove the need to create a stream for parsing strings
	template <>
	inline void unformat_arg<std::string>(const char* input, std::size_t size, std::string& output)
	{
		output.assign(input, size);
	}

	// Empty function to end recursion, no more args to process
	inline void unformat(std::size_t inputPos, std::size_t formatPos, const std::string& input, const std::string& format)
	{
	}

	template <typename T, typename... TRest>
	void unformat(std::size_t inputPos, std::size_t formatPos, const std::string& input, const std::string& format, T& first, TRest&... rest) noexcept
	{
		// Find {}
		const auto formatStart = format.find("{}", formatPos);
		if (formatStart == std::string::npos)
		{
			return;
		}

		// Find input string
		inputPos += formatStart - formatPos;
		auto inputEndChar = format[formatStart + 2];
		const std::size_t inputEnd = [&]() {
			if (inputEndChar == '\0')
			{
				return input.length();
			}
			else
			{
				return input.find(inputEndChar, inputPos);
			}
		}();

		// Process this arg
		unformat_arg(&input[inputPos], inputEnd - inputPos, first);

		// Process TRest
		unformat(inputEnd, formatStart + 2, input, format, rest...);
	}

	// Parses and extracts data from 'input' given a braced styled "{}" 'format' into 'args...'
	// For example:
	//     std::string name, int age;
	//     unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
	//
	// Then the following data is extracted:
	//     name == "Harry" and age == 18
	template <typename... Args>
	void unformat(const std::string& input, const std::string& format, Args&... args) noexcept
	{
		unformat(0, 0, input, format, args...);
	}
}
