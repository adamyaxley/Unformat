#pragma once

#ifdef _MSC_VER
#if _MSC_VER >= 1910 && _HAS_CXX17
#define UNFORMAT_CPP17
#endif
#endif

#include <sstream>
#ifdef UNFORMAT_CPP17
#include <string_view>
#endif

namespace
{
	template <typename T>
	void unformat_signed_int(const char* input, std::size_t size, T& output)
	{
		output = 0;
		int dec = 1;
		if (input[0] == '-')
		{
			dec = -1;
			input++;
			size--;
		}
		for (int i = size - 1; i >= 0; i--)
		{
			output += (input[i] - '0') * dec;
			dec *= 10;
		}
	}

	template <typename T>
	void unformat_unsigned_int(const char* input, std::size_t size, T& output)
	{
		output = 0;
		int dec = 1;
		for (int i = size - 1; i >= 0; i--)
		{
			output += (input[i] - '0') * dec;
			dec *= 10;
		}
	}

	template <typename T>
	void unformat_real(const char* input, std::size_t size, T& output)
	{
		output = 0;
		int dec = 1;
		if (input[0] == '-')
		{
			dec = -1;
			input++;
			size--;
		}

		for (int i = size - 1; i >= 0; i--)
		{
			if (input[i] == '.' && output != 0.0f)
			{
				output /= dec;
				dec = 1;
				continue;
			}
			output += (input[i] - '0') * dec;
			dec *= 10;
		}
	}
}

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

	template <>
	inline void unformat_arg<char>(const char* input, std::size_t size, char& output)
	{
		output = input[0];
	}

#ifdef UNFORMAT_CPP17
	template <>
	inline void unformat_arg<std::string_view>(const char* input, std::size_t size, std::string_view& output)
	{
		std::string_view newOutput(input, size);
		std::swap(output, newOutput);
	}
#endif

	template <>
	inline void unformat_arg<std::string>(const char* input, std::size_t size, std::string& output)
	{
		output.assign(input, size);
	}

	template <>
	inline void unformat_arg<float>(const char* input, std::size_t size, float& output)
	{
		unformat_real<float>(input, size, output);
	}

	template <>
	inline void unformat_arg<int>(const char* input, std::size_t size, int& output)
	{
		unformat_signed_int<int>(input, size, output);
	}

	template <>
	inline void unformat_arg<unsigned int>(const char* input, std::size_t size, unsigned int& output)
	{
		unformat_unsigned_int<unsigned int>(input, size, output);
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
