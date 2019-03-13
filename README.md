**[Source available on GitHub](https://github.com/adamyaxley/Unformat)**

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
Run on (16 X 2993 MHz CPU s)
03/13/19 18:10:57
--------------------------------------------------------------------
Benchmark                             Time           CPU Iterations
--------------------------------------------------------------------
Unformat                             72 ns         71 ns    8960000
Unformat_ConstChar                   69 ns         70 ns    8960000
Unformat_ConstexprMakeFormat         36 ns         35 ns   19478261
StdStringStream                     844 ns        854 ns     896000
StdRegex                           9975 ns      10010 ns      64000
StdScanf                           1716 ns       1726 ns     407273
```
