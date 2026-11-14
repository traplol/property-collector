#pragma once

#include "properties.h"

// Bar deliberately has different field names and a nested quantity.
struct BarThing
{
  std::string label;
  double unit_price = 0;
  struct Inventory
  {
    int count = 0;
  } inventory;
};

namespace accessor
{
template<>
struct Accessor<property::Name, BarThing>
{
  std::string operator()(const BarThing& value) const { return value.label; }
};

template<>
struct Accessor<property::Price, BarThing>
{
  double operator()(const BarThing& value) const { return value.unit_price; }
};

template<>
struct Accessor<property::Quantity, BarThing>
{
  int operator()(const BarThing& value) const { return value.inventory.count; }
};
}
