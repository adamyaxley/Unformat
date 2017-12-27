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
12/27/17 21:18:30
--------------------------------------------------------------------
Benchmark                             Time           CPU Iterations
--------------------------------------------------------------------
Unformat                            503 ns        502 ns    1120000
Unformat_ConstexprMakeFormat        133 ns        132 ns    4977778
StdStringStream                    3866 ns       3836 ns     179200
StdRegex                          32443 ns      32785 ns      22400
StdScanf                           6888 ns       6836 ns     112000
```
