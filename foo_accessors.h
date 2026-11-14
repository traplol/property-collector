#pragma once

#include "properties.h"

struct FooThing
{
  std::string name;
  double price = 0;
  int qty = 0;
};

namespace accessor
{
template<>
struct Accessor<property::Name, FooThing>
{
  std::string operator()(const FooThing& value) const { return value.name; }
};

template<>
struct Accessor<property::Price, FooThing>
{
  double operator()(const FooThing& value) const { return value.price; }
};

template<>
struct Accessor<property::Quantity, FooThing>
{
  int operator()(const FooThing& value) const { return value.qty; }
};
}
