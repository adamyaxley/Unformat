#pragma once

#ifdef _MSC_VER
#define UNFORMAT_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define UNFORMAT_FORCEINLINE __attribute__((always_inline)) inline
#else
#define UNFORMAT_FORCEINLINE inline
#endif

#ifdef _MSC_VER
#define UNFORMAT_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define UNFORMAT_NOINLINE __attribute__((noinline))
#else
#define UNFORMAT_NOINLINE
#endif

#ifdef _MSC_VER
#define UNFORMAT_RESTRICT __restrict
#else
#define UNFORMAT_RESTRICT __restrict__
#endif

#include <cstddef> // std::size_t (freestanding)

namespace ay
{
	// Non-owning string reference for extracted text.
	// Intentionally minimal — copy data and size to your own type after parsing.
	struct string_view
	{
		const char* data;
		std::size_t size;
	};
}

namespace
{
	UNFORMAT_FORCEINLINE std::size_t unformat_strlen(const char* s) noexcept
	{
		const char* p = s;
		while (*p) ++p;
		return static_cast<std::size_t>(p - s);
	}

	UNFORMAT_FORCEINLINE const char* unformat_strchr(const char* s, char c) noexcept
	{
		while (*s != c)
		{
			if (*s == '\0') return nullptr;
			++s;
		}
		return s;
	}

	UNFORMAT_NOINLINE double unformat_strtod(const char* buf, const char* bufEnd) noexcept
	{
		// String-to-double for scientific notation and long inputs.
		// Accumulates up to 18 significant digits to stay within ULL precision,
		// then tracks remaining digits as implicit exponent.
		const char* p = buf;
		bool negative = false;
		if (p < bufEnd && *p == '-') { negative = true; ++p; }
		else if (p < bufEnd && *p == '+') { ++p; }

		unsigned long long mantissa = 0;
		int exponent = 0;
		int sig_digits = 0;
		const int MAX_SIG = 18; // fits in unsigned long long

		// Integer part
		while (p < bufEnd && static_cast<unsigned>(*p - '0') <= 9u)
		{
			if (sig_digits < MAX_SIG)
			{
				mantissa = mantissa * 10 + static_cast<unsigned>(*p - '0');
				++sig_digits;
			}
			else
			{
				// Digit beyond precision — just count it as implicit exponent
				++exponent;
			}
			++p;
		}

		// Fractional part
		if (p < bufEnd && *p == '.')
		{
			++p;
			while (p < bufEnd && static_cast<unsigned>(*p - '0') <= 9u)
			{
				if (sig_digits < MAX_SIG)
				{
					mantissa = mantissa * 10 + static_cast<unsigned>(*p - '0');
					++sig_digits;
					--exponent;
				}
				// Digits beyond precision are silently dropped
				++p;
			}
		}

		// Scientific notation exponent
		if (p < bufEnd && (*p == 'e' || *p == 'E'))
		{
			++p;
			bool exp_neg = false;
			if (p < bufEnd && *p == '-') { exp_neg = true; ++p; }
			else if (p < bufEnd && *p == '+') { ++p; }

			int exp = 0;
			while (p < bufEnd && static_cast<unsigned>(*p - '0') <= 9u)
			{
				exp = exp * 10 + (*p - '0');
				++p;
			}
			exponent += exp_neg ? -exp : exp;
		}

		// Apply combined exponent with exact pow10 lookup tables.
		// pow10_exact[0..22] are exactly representable in double.
		// pow10_22x[k] = 1e(22*k) for large exponents, allowing any
		// exponent up to 308 to be applied in at most 2 multiplications.
		double result = static_cast<double>(mantissa);
		if (exponent != 0)
		{
			static const double pow10_exact[] = {
				1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
				1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
				1e20, 1e21, 1e22
			};
			static const double pow10_22x[] = {
				1e0, 1e22, 1e44, 1e66, 1e88, 1e110, 1e132, 1e154,
				1e176, 1e198, 1e220, 1e242, 1e264, 1e286, 1e308
			};
			int abs_exp = exponent < 0 ? -exponent : exponent;
			if (abs_exp <= 22)
			{
				// Single exact multiply
				result = exponent > 0 ? result * pow10_exact[abs_exp] : result / pow10_exact[abs_exp];
			}
			else if (abs_exp <= 308)
			{
				// Split: exact remainder * precomputed multiple-of-22
				int remainder = abs_exp % 22;
				int multiple = abs_exp / 22;
				if (exponent > 0)
				{
					result *= pow10_exact[remainder];
					result *= pow10_22x[multiple];
				}
				else
				{
					result /= pow10_exact[remainder];
					result /= pow10_22x[multiple];
				}
			}
			else
			{
				// Beyond double range — will become 0 or inf
				if (exponent > 0)
				{
					result *= pow10_22x[14]; // 1e308
					int remaining = abs_exp - 308;
					while (remaining > 0)
					{
						int step = remaining > 308 ? 308 : remaining;
						int r = step % 22;
						int m = step / 22;
						result *= pow10_exact[r];
						result *= pow10_22x[m];
						remaining -= step;
					}
				}
				else
				{
					result /= pow10_22x[14]; // 1e308
					int remaining = abs_exp - 308;
					while (remaining > 0)
					{
						int step = remaining > 308 ? 308 : remaining;
						int r = step % 22;
						int m = step / 22;
						result /= pow10_exact[r];
						result /= pow10_22x[m];
						remaining -= step;
					}
				}
			}
		}

		return negative ? -result : result;
	}

	template <typename T>
	UNFORMAT_FORCEINLINE void unformat_signed_int(const char* input, const char* inputEnd, T& output) noexcept
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
	UNFORMAT_FORCEINLINE void unformat_unsigned_int(const char* input, const char* inputEnd, T& output) noexcept
	{
		output = 0;
		while (input != inputEnd)
		{
			output *= 10;
			output += (*input - '0');
			++input;
		}
	}

	// Lookup table for negative powers of 10, used to convert integer mantissa
	// to floating point with a single multiply at the end
	static const double unformat_pow10_neg[] = {
		1.0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9,
		1e-10, 1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17, 1e-18, 1e-19, 1e-20
	};

	template <typename T>
	void unformat_real(const char* input, const char* inputEnd, T& output) noexcept
	{
		const auto inputStart = input;
		bool negative = false;

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

		// Parse all digits as integer mantissa (fast integer multiply+add)
		// Accumulate up to 18 significant digits, track remainder as exponent
		unsigned long long mantissa = 0;
		int exponent = 0;
		int sig_digits = 0;
		const int MAX_SIG = 18;

		// Integer part
		for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
		{
			if (sig_digits < MAX_SIG)
			{
				mantissa = mantissa * 10 + d;
				++sig_digits;
			}
			else
			{
				++exponent; // digit beyond precision, track as implicit exponent
			}
		}

		// Decimal part
		if (*input == '.')
		{
			++input;
			for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
			{
				if (sig_digits < MAX_SIG)
				{
					mantissa = mantissa * 10 + d;
					++sig_digits;
					--exponent;
				}
			}
		}

		// Scientific notation or exponent too large for direct lookup: use strtod
		bool use_strtod = (input != inputEnd && (*input == 'e' || *input == 'E'));

		// Convert mantissa to double and apply exponent
		double result = static_cast<double>(mantissa);
		if (!use_strtod && exponent != 0)
		{
			int abs_exp = exponent < 0 ? -exponent : exponent;
			if (abs_exp <= 20)
			{
				result = exponent > 0 ? result * (1.0 / unformat_pow10_neg[abs_exp]) : result * unformat_pow10_neg[abs_exp];
			}
			else
			{
				use_strtod = true;
			}
		}

		if (use_strtod)
		{
			output = static_cast<T>(unformat_strtod(inputStart, inputEnd));
			return;
		}

		if (negative)
		{
			result = -result;
		}

		output = static_cast<T>(result);
	}

	// --- Combined scan+parse helpers for numeric types ---
	// These scan for endChar while parsing in a single pass,
	// eliminating the redundant scan loop in unformat_internal.

	template <typename T>
	UNFORMAT_FORCEINLINE const char* unformat_signed_int_scan(const char* UNFORMAT_RESTRICT input, T& output) noexcept
	{
		T val = 0;
		int sign = 1;
		if (*input == '-')
		{
			sign = -1;
			++input;
		}

#ifndef UNFORMAT_DISABLE_PLUS_SIGN
		if (*input == '+')
		{
			++input;
		}
#endif

		// Digit-range check: stops at any non-digit (including endCh)
		for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
		{
			val = val * 10 + static_cast<T>(d);
		}
		output = val * sign;
		return input;
	}

	template <typename T>
	UNFORMAT_FORCEINLINE const char* unformat_unsigned_int_scan(const char* UNFORMAT_RESTRICT input, T& output) noexcept
	{
		T val = 0;
		// Digit-range check: stops at any non-digit (including endCh)
		for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
		{
			val = val * 10 + static_cast<T>(d);
		}
		output = val;
		return input;
	}

	// Cold path: fallback for scientific notation or very long float inputs
	template <typename T>
	UNFORMAT_NOINLINE const char* unformat_real_scan_slow(const char* inputStart, const char* inputCurrent, char endCh, T& output) noexcept
	{
		const char* end = inputCurrent;
		while (*end != endCh) ++end;
		output = static_cast<T>(unformat_strtod(inputStart, end));
		return end;
	}

	template <typename T>
	UNFORMAT_FORCEINLINE const char* unformat_real_scan(const char* UNFORMAT_RESTRICT input, char endCh, T& output) noexcept
	{
		const char* inputStart = input;
		bool negative = false;

		if (*input == '-')
		{
			negative = true;
			++input;
		}

#ifndef UNFORMAT_DISABLE_PLUS_SIGN
		if (*input == '+')
		{
			++input;
		}
#endif

		// Parse all digits as integer mantissa (fast integer multiply+add)
		unsigned long long mantissa = 0;
		int decimal_digits = 0;
		const char* digitStart = input;

		// Integer part — single digit-range check replaces 4 comparisons
		for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
		{
			mantissa = mantissa * 10 + d;
		}

		// Decimal part
		if (*input == '.')
		{
			++input;
			for (unsigned d; (d = static_cast<unsigned>(*input - '0')) <= 9u; ++input)
			{
				mantissa = mantissa * 10 + d;
				++decimal_digits;
			}
		}

		// Scientific notation or very long input (>18 total digits overflows ULL): cold path fallback
		if (*input == 'e' || *input == 'E' ||
			static_cast<int>(input - digitStart) - (decimal_digits > 0) > 18)
		{
			return unformat_real_scan_slow(inputStart, input, endCh, output);
		}

		// Single FP conversion: cast mantissa to double, scale by power of 10
		double result = static_cast<double>(mantissa) * unformat_pow10_neg[decimal_digits];

		if (negative)
		{
			result = -result;
		}

		output = static_cast<T>(result);
		return input;
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

	template <typename T>
	T unformat_arg(const char* input) noexcept
	{
		return unformat_arg<T>(input, input + unformat_strlen(input));
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

	template <>
	inline void unformat_arg<ay::string_view>(const char* input, const char* inputEnd, ay::string_view& output) noexcept
	{
		output.data = input;
		output.size = static_cast<std::size_t>(inputEnd - input);
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

	// --- Combined scan+parse: unformat_arg_scan ---
	// Scans for the endChar while parsing the value in a single pass,
	// eliminating the redundant scan loop. Returns pointer to endChar position.
	// Default: scan then call unformat_arg (for user-defined types)
	template <typename T>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan(const char* input, char endCh, T& output) noexcept
	{
		const char* end = unformat_strchr(input, endCh);
		unformat_arg(input, end, output);
		return end;
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<char>(const char* input, char endCh, char& output) noexcept
	{
		output = input[0];
		return unformat_strchr(input, endCh);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<unsigned char>(const char* input, char endCh, unsigned char& output) noexcept
	{
		output = input[0];
		return unformat_strchr(input, endCh);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<ay::string_view>(const char* input, char endCh, ay::string_view& output) noexcept
	{
		const char* UNFORMAT_RESTRICT end = input;
		while (*end != endCh)
		{
			++end;
		}
		output.data = input;
		output.size = static_cast<std::size_t>(end - input);
		return end;
	}

	// Signed integer types: combined scan+parse in one loop
	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<short>(const char* input, char endCh, short& output) noexcept
	{
		return unformat_signed_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<int>(const char* input, char endCh, int& output) noexcept
	{
		return unformat_signed_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<long>(const char* input, char endCh, long& output) noexcept
	{
		return unformat_signed_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<long long>(const char* input, char endCh, long long& output) noexcept
	{
		return unformat_signed_int_scan(input, output);
	}

	// Unsigned integer types: combined scan+parse in one loop
	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<unsigned short>(const char* input, char endCh, unsigned short& output) noexcept
	{
		return unformat_unsigned_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<unsigned int>(const char* input, char endCh, unsigned int& output) noexcept
	{
		return unformat_unsigned_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<unsigned long>(const char* input, char endCh, unsigned long& output) noexcept
	{
		return unformat_unsigned_int_scan(input, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<unsigned long long>(const char* input, char endCh, unsigned long long& output) noexcept
	{
		return unformat_unsigned_int_scan(input, output);
	}

	// Float/double: combined scan+parse
	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<float>(const char* input, char endCh, float& output) noexcept
	{
		return unformat_real_scan(input, endCh, output);
	}

	template <>
	UNFORMAT_FORCEINLINE const char* unformat_arg_scan<double>(const char* input, char endCh, double& output) noexcept
	{
		return unformat_real_scan(input, endCh, output);
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
			*this = make_format_non_template(str, unformat_strlen(str));
		}

		static constexpr std::size_t MAX_COUNT{ 16 };
		unsigned char offsets[MAX_COUNT]{};
		char endChar[MAX_COUNT]{};
		unsigned char count{ 0 };
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
					throw "Max number of {} has been exceeded.";
				}
				format.offsets[format.count] = static_cast<unsigned char>(to - from);
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
	template <std::size_t ArgIndex>
	UNFORMAT_FORCEINLINE void unformat_internal(const char* input, const format& format) noexcept
	{
	}

	template <std::size_t ArgIndex, typename T, typename... TRest>
	UNFORMAT_FORCEINLINE void unformat_internal(const char* input, const format& format, T& first, TRest&... rest) noexcept
	{
		// Advance input by the precomputed offset (compile-time index)
		input += format.offsets[ArgIndex];

		// Combined scan+parse: find endChar while parsing value in one pass
		const char* end = unformat_arg_scan(input, format.endChar[ArgIndex], first);

		// Process TRest with compile-time incremented index
		unformat_internal<ArgIndex + 1>(end, format, rest...);
	}

	// Parses and extracts data from 'input' given a braced styled "{}" 'format' into 'args...'
	// For example:
	//     ay::string_view name; int age;
	//     constexpr auto format = make_format("{} is {} years old.");
	//     unformat("Harry is 18 years old.", format, name, age);
	//
	// Then the following data is extracted:
	//     name == "Harry" and age == 18
	template <typename... Args>
	void unformat(const char* input, const format& format, Args&... args) noexcept
	{
		unformat_internal<0>(input, format, args...);
	}
}
