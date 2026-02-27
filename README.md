**[Source available on GitHub](https://github.com/adamyaxley/Unformat)**

# Unformat
Parsing and extraction of original data from brace style "{}" formatted strings. It basically _unformats_ what you thought was formatted for good.

## Quick Example
Unformat is simple to use and works on all basic types. See the below example for extracting an `ay::string_view` and an `int`
```c++
ay::string_view name;
int age;

unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
// name == "Harry" and age == 18
```
As an optimisation, if the format string is known at compile time, it can be parsed into a constant expression by making use of `ay::make_format`. In tests, this increases runtime speed by a factor of 3.
```c++
ay::string_view name;
int age;

constexpr auto format = ay::make_format("{} is {} years old.");
unformat("Harry is 18 years old.", format, name, age);
// name == "Harry" and age == 18
```

## How do I use this library?
Unformat is a single-file header only library with zero dependencies (only `<cstddef>` for `std::size_t`). All you need to do is copy `unformat.h` into your project, and away you go.

`ay::string_view` is a simple non-owning struct containing a `const char* data` pointer and a `std::size_t size`. It can be converted to your own string type after parsing.

## Public Domain
This software is completely open source and in the public domain. See LICENSE for details.

## Contributing
Pull requests are very welcome. You may also create Issues and I will have a look into it as soon as I can.

## Speed
Unformat is super awesome back to the future style lightning fast compared to traditional parsing methods. Below is the output from Google Benchmark on unformat_benchmark.cpp. Great Scott!
```
Run on (32 X 2420 MHz CPU s)
---------------------------------------------------------
Benchmark                                Time
---------------------------------------------------------
Unformat                              32.6 ns
Unformat_ConstexprMakeFormat          11.5 ns
StdStringStream                        391 ns
StdRegex                              3269 ns
StdScanf                               677 ns
```
