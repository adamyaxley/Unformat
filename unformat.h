#pragma once

#ifdef _MSC_VER
#if _MSC_VER >= 1910 && _HAS_CXX17
#define UNFORMAT_CPP17
#endif
#endif

#include <string>
#ifdef UNFORMAT_CPP17
#include <string_view>
#endif

namespace
{
	template <typename T>
	void unformat_signed_int(const char* input, const char* inputEnd, T& output)
	{
		output = 0;
		int dec = 1;
		if (*input == '-')
		{
			dec = -1;
			input++;
		}

		while (input != inputEnd)
		{
			--inputEnd;
			output += (*inputEnd - '0') * dec;
			dec *= 10;
		}
	}

	template <typename T>
	void unformat_unsigned_int(const char* input, const char* inputEnd, T& output)
	{
		output = 0;
		int dec = 1;

		while (input != inputEnd)
		{
			--inputEnd;
			output += (*inputEnd - '0') * dec;
			dec *= 10;
		}
	}

	template <typename T>
	void unformat_real(const char* input, const char* inputEnd, T& output)
	{
		output = 0;
		int dec = 1;
		if (*input == '-')
		{
			dec = -1;
			input++;
		}

		while (input != inputEnd)
		{
			--inputEnd;
			if (*inputEnd == '.' && output != 0.0f)
			{
				output /= dec;
				dec = 1;
				continue;
			}
			output += (*inputEnd - '0') * dec;
			dec *= 10;
		}
	}
}

namespace ay
{
	// Not defined on purpose for the general case. Note that you can define custom unformatters by defining new specialisations
	// for this function.
	template <typename T>
	void unformat_arg(const char* input, const char* inputEnd, T& output);

	template <>
	inline void unformat_arg<char>(const char* input, const char* inputEnd, char& output)
	{
		output = input[0];
	}

	template <>
	inline void unformat_arg<unsigned char>(const char* input, const char* inputEnd, unsigned char& output)
	{
		output = input[0];
	}

#ifdef UNFORMAT_CPP17
	template <>
	inline void unformat_arg<std::string_view>(const char* input, const char* inputEnd, std::string_view& output)
	{
		std::string_view newOutput(input, inputEnd - input);
		std::swap(output, newOutput);
	}
#endif

	template <>
	inline void unformat_arg<std::string>(const char* input, const char* inputEnd, std::string& output)
	{
		output.assign(input, inputEnd - input);
	}

	template <>
	inline void unformat_arg<float>(const char* input, const char* inputEnd, float& output)
	{
		unformat_real<float>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<double>(const char* input, const char* inputEnd, double& output)
	{
		unformat_real<double>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<short>(const char* input, const char* inputEnd, short& output)
	{
		unformat_signed_int<short>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<int>(const char* input, const char* inputEnd, int& output)
	{
		unformat_signed_int<int>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<long>(const char* input, const char* inputEnd, long& output)
	{
		unformat_signed_int<long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<long long>(const char* input, const char* inputEnd, long long& output)
	{
		unformat_signed_int<long long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned short>(const char* input, const char* inputEnd, unsigned short& output)
	{
		unformat_unsigned_int<unsigned short>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned int>(const char* input, const char* inputEnd, unsigned int& output)
	{
		unformat_unsigned_int<unsigned int>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned long>(const char* input, const char* inputEnd, unsigned long& output)
	{
		unformat_unsigned_int<unsigned long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned long long>(const char* input, const char* inputEnd, unsigned long long& output)
	{
		unformat_unsigned_int<unsigned long long>(input, inputEnd, output);
	}

	struct format
	{
		static constexpr std::size_t MAX_COUNT{ 16 };
		std::size_t offsets[MAX_COUNT]{};
		char endChar[MAX_COUNT]{};
		std::size_t count{ 0 };
	};

	template <std::size_t N>
	constexpr format make_format(const char(&str)[N])
	{
		return make_format_non_template(str, N);
	}

	constexpr format make_format_non_template(const char* str, std::size_t N)
	{
		format format;

		std::size_t from = 0;
		for (std::size_t to = 0; to < N - 1; to++)
		{
			if (str[to] == '{' && str[to + 1] == '}')
			{
				if (format.count >= format::MAX_COUNT)
				{
					// Stops compilation in a constexpr context
					throw std::exception("Max number of {} has been exceeded.");
				}
				format.offsets[format.count] = to - from;
				format.endChar[format.count] = str[to + 2]; // May be '\0' which is fine
				++format.count;
				// Advance markers
				from = to + 2;
				to = from;
			}
		}

		return format;
	}

	// Empty function to end recursion, no more args to process
	inline void unformat_internal(std::size_t inputPos, const std::string& input, const format& format)
	{
	}

	template <typename T, typename... TRest>
	void unformat_internal(std::size_t inputPos, const std::string& input, const format& format, T& first, TRest&... rest) noexcept
	{
		const std::size_t argNo = format.count - sizeof...(rest) - 1;

		// Get the location of the first brace
		const auto offset = format.offsets[argNo];

		// Find input string
		inputPos += offset;
		auto inputEnd = inputPos + 1;
		while (input[inputEnd] != format.endChar[argNo])
		{
			++inputEnd;
		}

		// Process this arg
		unformat_arg(&input[inputPos], &input[inputEnd], first);

		// Process TRest
		unformat_internal(inputEnd, input, format, rest...);
	}

	// Parses and extracts data from 'input' given a braced styled "{}" 'format' into 'args...'
	// For example:
	//     std::string name, int age;
	//     constexpr auto format = make_format("{} is {} years old.");
	//     unformat("Harry is 18 years old.", format, name, age);
	//
	// Then the following data is extracted:
	//     name == "Harry" and age == 18
	template <typename... Args>
	void unformat(const std::string& input, const format& format, Args&... args) noexcept
	{
		unformat_internal(0, input, format, args...);
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
		unformat_internal(0, input, make_format_non_template(format.c_str(), format.size()), args...);
	}
}
