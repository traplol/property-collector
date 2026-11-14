// This translation unit has no BarThing declaration or Bar accessors.
#include "foo_accessors.h"

#include <cstdlib>
#include <iostream>

int main()
{
  collector::Collector<property::Name, property::Price, property::Quantity> col;
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(FooThing{"widget", 12.5, 8});
  col.Put(FooThing{"widget", 12.5, 4});

  if (col.Get<property::Name>() != "widget" ||
      col.Get<property::Price>() != 12.5 || col.Get<property::Quantity>()) {
    std::cerr << "Foo-only collection must preserve independent property consensus\n";
    return EXIT_FAILURE;
  }
  std::cout << "Foo-only collector checks passed\n";
}
