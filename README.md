Warning: this library is brand-spanking new and therefore untested and crawling with bugs.

# Unformat
Parsing and extraction of original data from brace style "{}" formatted strings. It basically _unformats_ what you thought was formatted for good.

## Quick Example

```c++
std::string name;
int age;

unformat("Harry is 18 years old.", "{} is {} years old.", name, age);
// name == "Harry" and age == 18
```

## Speed
Unformat is super awesome back to the future style lightning fast compared to traditional parsing methods. Below is the output from Google Benchmark on unformat_benchmark.cpp. Great Scott!
```
----------------------------------------------------
Benchmark             Time           CPU Iterations
----------------------------------------------------
Unformat            545 ns        544 ns    1120000
StdStringStream    3840 ns       3836 ns     179200
StdRegex          32004 ns      32087 ns      22400
```
