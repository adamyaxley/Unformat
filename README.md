Warning: this library is brand new and therefore untested and crawling with bugs.

# Unformat
Parsing and extraction of original data from brace style "{}" formatted strings. It basically _unformats_ what you thought was formatted for good.

## Quick Example
Unformat is simple to use and works on all basic types. See the below example for extracting a `std::string` and an 'int'
```c++
std::string name;
int age;

unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
// name == "Harry" and age == 18
```
As an optimisation, if the format string is known at compile time, it can be parsed into a constant expression by making use of `ay::make_format`. In tests, this increases runtime speed by a factor of 3 or 4.
```c++
std::string name;
int age;

constexpr auto format = ay::make_format("{} is {} years old.");
unformat("Harry is 18 years old.", format, name, age);
// name == "Harry" and age == 18
```

## How do I use this library?
Unformat is a single-file header only library so integration is easy. All you need to do is copy `unformat.h` into your project, and away you go.

## Public Domain
This software is completely open source and in the public domain. See LICENSE for details.

## Contributing
Pull requests are very welcome. You may also create Issues and I will have a look into it as soon as I can.

## Speed
Unformat is super awesome back to the future style lightning fast compared to traditional parsing methods. Below is the output from Google Benchmark on unformat_benchmark.cpp. Great Scott!
```
Run on (8 X 2594 MHz CPU s)
12/21/17 08:06:34
--------------------------------------------------------------------
Benchmark                             Time           CPU Iterations
--------------------------------------------------------------------
Unformat                            516 ns        516 ns    1120000
Unformat_ConstexprMakeFormat        166 ns        167 ns    3733333
StdStringStream                    3837 ns       3850 ns     186667
StdRegex                          30312 ns      29994 ns      22400
```
