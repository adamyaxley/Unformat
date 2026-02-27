#pragma once

#ifdef _MSC_VER
#if _MSC_VER >= 1910 && _HAS_CXX17
#define UNFORMAT_CPP17
#endif
#endif

#include <string>
#include <cstdlib>
#ifdef UNFORMAT_CPP17
#include <string_view>
#endif

namespace
{
	template <typename T>
	void unformat_signed_int(const char* input, const char* inputEnd, T& output) noexcept
	{
		output = 0;
		int sign = 1;
		if (*input == '-')
		{
			sign = -1;
			++input;
		}

#ifndef UNFORMAT_DISABLE_PLUS_SIGN
		// Ignore plus
		if (*input == '+')
		{
			++input;
		}
#endif

		while (input != inputEnd)
		{
			output *= 10;
			output += (*input - '0');
			++input;
		}
		output *= sign;
	}

	template <typename T>
	void unformat_unsigned_int(const char* input, const char* inputEnd, T& output) noexcept
	{
		output = 0;
		while (input != inputEnd)
		{
			output *= 10;
			output += (*input - '0');
			++input;
		}
	}

	template <typename T>
	void unformat_real(const char* input, const char* inputEnd, T& output) noexcept
	{
		const auto inputStart = input;
		const auto len = static_cast<std::size_t>(inputEnd - input);
		bool negative = false;

		// For very long inputs (e.g. %f formatted extreme values with hundreds
		// of digits), use strtod for correct IEEE 754 rounding
		if (len > 20)
		{
			char buf[400];
			const auto n = len < sizeof(buf) - 1 ? len : sizeof(buf) - 1;
			for (std::size_t i = 0; i < n; ++i) buf[i] = input[i];
			buf[n] = '\0';
			output = static_cast<T>(std::strtod(buf, nullptr));
			return;
		}

		// Fast manual parser for simple fixed-point numbers (e.g. "67.8")
		long double f = 0;

		// Check for negative
		if (*input == '-')
		{
			negative = true;
			++input;
		}

#ifndef UNFORMAT_DISABLE_PLUS_SIGN
		// Ignore plus
		if (*input == '+')
		{
			++input;
		}
#endif

		// Parse integer part
		while (*input != '.' && input != inputEnd && *input != 'e')
		{
			f *= 10;
			f += (*input - '0');
			++input;
		}

		// Parse decimal
		if (*input == '.')
		{
			++input;
			long double decimal = 1.0L;
			while (input != inputEnd && *input != 'e')
			{
				decimal *= 0.1L;
				f += (*input - '0') * decimal;
				++input;
			}
		}

		// For scientific notation, fall back to strtod for correct rounding
		// with large exponents where f * pow(10, e) would lose precision
		if (input != inputEnd && *input == 'e')
		{
			char buf[24];
			for (std::size_t i = 0; i < len; ++i) buf[i] = inputStart[i];
			buf[len] = '\0';
			output = static_cast<T>(std::strtod(buf, nullptr));
			return;
		}

		if (negative)
		{
			f = -f;
		}

		output = static_cast<T>(f);
	}
}

namespace ay
{
	// Not defined on purpose for the general case. Note that you can define custom unformatters by defining new specialisations
	// for this function.
	template <typename T>
	void unformat_arg(const char* input, const char* inputEnd, T& output) noexcept;

	template <typename T>
	T unformat_arg(const char* input, const char* inputEnd) noexcept
	{
		T output;
		unformat_arg(input, inputEnd, output);
		return output;
	}

#ifdef UNFORMAT_CPP17
	template <typename T>
	T unformat_arg(std::string_view input) noexcept
	{
		return unformat_arg<T>(input.data(), input.data() + input.length());
	}
#endif

	template <typename T>
	T unformat_arg(std::string input) noexcept
	{
		return unformat_arg<T>(input.c_str(), input.c_str() + input.length());
	}

	template <>
	inline void unformat_arg<char>(const char* input, const char* inputEnd, char& output) noexcept
	{
		output = input[0];
	}

	template <>
	inline void unformat_arg<unsigned char>(const char* input, const char* inputEnd, unsigned char& output) noexcept
	{
		output = input[0];
	}

#ifdef UNFORMAT_CPP17
	template <>
	inline void unformat_arg<std::string_view>(const char* input, const char* inputEnd, std::string_view& output) noexcept
	{
		output = std::string_view(input, inputEnd - input);
	}
#endif

	template <>
	inline void unformat_arg<std::string>(const char* input, const char* inputEnd, std::string& output) noexcept
	{
		output.assign(input, inputEnd - input);
	}

	template <>
	inline void unformat_arg<float>(const char* input, const char* inputEnd, float& output) noexcept
	{
		unformat_real<float>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<double>(const char* input, const char* inputEnd, double& output) noexcept
	{
		unformat_real<double>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<short>(const char* input, const char* inputEnd, short& output) noexcept
	{
		unformat_signed_int<short>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<int>(const char* input, const char* inputEnd, int& output) noexcept
	{
		unformat_signed_int<int>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<long>(const char* input, const char* inputEnd, long& output) noexcept
	{
		unformat_signed_int<long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<long long>(const char* input, const char* inputEnd, long long& output) noexcept
	{
		unformat_signed_int<long long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned short>(const char* input, const char* inputEnd, unsigned short& output) noexcept
	{
		unformat_unsigned_int<unsigned short>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned int>(const char* input, const char* inputEnd, unsigned int& output) noexcept
	{
		unformat_unsigned_int<unsigned int>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned long>(const char* input, const char* inputEnd, unsigned long& output) noexcept
	{
		unformat_unsigned_int<unsigned long>(input, inputEnd, output);
	}

	template <>
	inline void unformat_arg<unsigned long long>(const char* input, const char* inputEnd, unsigned long long& output) noexcept
	{
		unformat_unsigned_int<unsigned long long>(input, inputEnd, output);
	}

	struct format;

	constexpr format make_format_non_template(const char* str, std::size_t N);

	struct format
	{
		constexpr format() {}

		template <std::size_t N>
		constexpr format(const char(&str)[N])
		{
			*this = make_format_non_template(str, N);
		}

		format(const char* str)
		{
			*this = make_format_non_template(str, std::strlen(str));
		}

		static constexpr std::size_t MAX_COUNT{ 16 };
		std::size_t offsets[MAX_COUNT]{};
		char endChar[MAX_COUNT]{};
		std::size_t count{ 0 };
	};

	template <std::size_t N>
	constexpr format make_format(const char(&str)[N]) noexcept
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
				to = from - 1;
			}
		}

		return format;
	}

	// Empty function to end recursion, no more args to process
	inline void unformat_internal(std::size_t inputPos, const char* input, const format& format) noexcept
	{
	}

	template <typename T, typename... TRest>
	void unformat_internal(std::size_t inputPos, const char* input, const format& format, T& first, TRest&... rest) noexcept
	{
		const std::size_t argNo = format.count - sizeof...(rest) - 1;

		// Get the location of the first brace
		const auto offset = format.offsets[argNo];

		// Find input string
		inputPos += offset;
		auto inputEnd = inputPos;
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
	void unformat(const char* input, const format& format, Args&... args) noexcept
	{
		unformat_internal(0, input, format, args...);
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
		unformat_internal(0, input.c_str(), format, args...);
	}
}
