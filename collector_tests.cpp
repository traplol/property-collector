#include "foo_accessors.h"
#include "bar_accessors.h"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

// Same result type as Name, but a separate property.
struct Initial : accessor::Property<std::string> {};

struct CanonicalValue : accessor::Property<int> {};
struct RawValue
{
  double value;
};

namespace accessor
{
template<>
struct Accessor<CanonicalValue, RawValue>
{
  double operator()(const RawValue& value) const { return value.value; }
};

template<>
struct Accessor<Initial, FooThing>
{
  std::string operator()(const FooThing& value) const { return value.name.substr(0, 1); }
};

template<>
struct Accessor<Initial, BarThing>
{
  std::string operator()(const BarThing& value) const { return value.label.substr(0, 1); }
};
}

namespace
{
using property::Name;
using property::Price;
using property::Quantity;
using Collector = collector::Collector<Name, Initial, Price, Quantity>;

void Check(bool condition, const char* message)
{
  if (!condition) {
    std::cerr << message << '\n';
    std::exit(EXIT_FAILURE);
  }
}

void CheckConsensus(const Collector& col,
                    const std::optional<std::string>& name,
                    const std::optional<std::string>& initial,
                    const std::optional<double>& price,
                    const std::optional<int>& quantity,
                    const char* message)
{
  Check(col.Get<Name>() == name && col.Get<Initial>() == initial &&
          col.Get<Price>() == price && col.Get<Quantity>() == quantity,
        message);
}

template<typename Value>
void CheckSourceDisagreements(const Value& quantityChange,
                             const Value& nameChange,
                             const Value& initialChange,
                             const Value& priceChange)
{
  Collector col;
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(BarThing{"widget", 12.5, {4}});
  CheckConsensus(col, "widget", "w", 12.5, 4, "Both sources must agree initially");

  col.Put(quantityChange);
  CheckConsensus(col, "widget", "w", 12.5, std::nullopt,
                 "A quantity disagreement must leave all other consensus intact");
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(BarThing{"widget", 12.5, {4}});
  CheckConsensus(col, "widget", "w", 12.5, std::nullopt,
                 "Matching inputs must not restore quantity consensus");

  col.Put(nameChange);
  CheckConsensus(col, std::nullopt, "w", 12.5, std::nullopt,
                 "A name disagreement must preserve Initial and Price consensus");
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(BarThing{"widget", 12.5, {4}});
  CheckConsensus(col, std::nullopt, "w", 12.5, std::nullopt,
                 "Matching inputs must not restore name consensus");

  col.Put(initialChange);
  CheckConsensus(col, std::nullopt, std::nullopt, 12.5, std::nullopt,
                 "An initial disagreement must preserve price consensus");
  col.Put(priceChange);
  CheckConsensus(col, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 "A price disagreement must clear price consensus");
  col.Put(FooThing{"widget", 12.5, 4});
  col.Put(BarThing{"widget", 12.5, {4}});
  CheckConsensus(col, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 "Later matching values must not restore any lost consensus");

  Collector priceOnly;
  priceOnly.Put(FooThing{"widget", 12.5, 4});
  priceOnly.Put(BarThing{"widget", 12.5, {4}});
  priceOnly.Put(priceChange);
  CheckConsensus(priceOnly, "widget", "w", std::nullopt, 4,
                 "A price-only disagreement must preserve every other property");
  priceOnly.Put(FooThing{"widget", 12.5, 4});
  priceOnly.Put(BarThing{"widget", 12.5, {4}});
  CheckConsensus(priceOnly, "widget", "w", std::nullopt, 4,
                 "Matching inputs must not restore price consensus");
}
}

int main()
{
  Collector col;
  CheckConsensus(col, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                 "Empty input must have no consensus");

  col.Put(FooThing{"", 0, 0});
  CheckConsensus(col, "", "", 0, 0,
                 "A single input must establish consensus, including empty strings and zero");
  col.Put(BarThing{"", 0, {0}});
  CheckConsensus(col, "", "", 0, 0,
                 "Matching values from another type must preserve consensus");

  Collector mixed;
  mixed.Put(BarThing{"widget", 12.5, {4}});
  for (int i = 0; i < 100; ++i) {
    mixed.Put(FooThing{"widget", 12.5, 4});
    mixed.Put(BarThing{"widget", 12.5, {4}});
  }
  CheckConsensus(mixed, "widget", "w", 12.5, 4,
                 "Any number of matching inputs in either order must preserve consensus");

  CheckSourceDisagreements(FooThing{"widget", 12.5, 8},
                           FooThing{"washer", 12.5, 4},
                           FooThing{"gadget", 12.5, 4},
                           FooThing{"widget", 15, 4});
  CheckSourceDisagreements(BarThing{"widget", 12.5, {8}},
                           BarThing{"washer", 12.5, {4}},
                           BarThing{"gadget", 12.5, {4}},
                           BarThing{"widget", 15, {4}});

  collector::Collector<CanonicalValue> converted;
  converted.Put(RawValue{1.25});
  Check(converted.Get<CanonicalValue>() == 1, "The first result must convert to result_t");
  converted.Put(RawValue{1.25});
  Check(converted.Get<CanonicalValue>() == 1,
        "Identical inputs must retain consensus after conversion to result_t");
  converted.Put(RawValue{1.75});
  Check(converted.Get<CanonicalValue>() == 1,
        "Inputs that convert to the same result must preserve consensus");
  converted.Put(RawValue{2.25});
  Check(!converted.Get<CanonicalValue>(), "Different converted results must lose consensus");
  converted.Put(RawValue{1.25});
  Check(!converted.Get<CanonicalValue>(), "Converted results must also have sticky disagreement");

  const Collector& result = col;
  Check(result.Get<Quantity>() == 0, "Consensus must be readable through a const collector");
  std::cout << "All collector checks passed\n";
}
