#include <iostream>
#include "foo_accessors.h"
#include "bar_accessors.h"

using Col = collector::Collector<property::Name, property::Price, property::Quantity>;

template<typename Property>
void Print(const char* label, const Col& col)
{
  std::cout << label << ": ";
  if (const auto& value = col.Get<Property>()) {
    std::cout << *value;
  }
  else {
    std::cout << "no consensus";
  }
  std::cout << '\n';
}

auto main() -> int
{
  Col col;
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(BarThing{"widget", 12.5, {8}});
  col.Put(FooThing{"widget", 12.5, 4});

  Print<property::Name>("Name", col);
  Print<property::Price>("Price", col);
  Print<property::Quantity>("Quantity", col);
  return 0;
}
