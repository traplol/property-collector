# Property collector POC

A C++17 collector that keeps per-property consensus across any number of objects
of different types. The example combines `FooThing` and `BarThing`.

```sh
cmake -S . -B /tmp/property-collector-build
cmake --build /tmp/property-collector-build
ctest --test-dir /tmp/property-collector-build --output-on-failure
/tmp/property-collector-build/property_collector
```

The example prints:

```text
Name: widget
Price: 12.5
Quantity: no consensus
```

Define a property identifier with `result_t`, or inherit from
`accessor::Property<ResultT>`. List the identifiers in `collector::Collector<...>`.
For each source type, specialize `accessor::Accessor<Property, Source>` with a
const `operator()` that extracts that property. Each accessor must be default
constructible and its result must support equality.
Extracted values convert to the property's `result_t` before storage and comparison.

`properties.h` defines shared property identifiers without referencing a source
type. `foo_accessors.h` and `bar_accessors.h` define the source-specific mappings
independently. They deliberately read different field names and structures.
Each accessor has a single `operator()`, with no overload for the other type.

Foo-only code includes only `foo_accessors.h`:

```cpp
#include "foo_accessors.h"

collector::Collector<property::Name, property::Quantity> col;
col.Put(FooThing{"widget", 12.5, 4});
col.Put(FooThing{"widget", 12.5, 8});
// col.Get<property::Name>() holds "widget".
// col.Get<property::Quantity>() is empty.
```

Mixed code includes both accessor headers and uses the same collector type.
Only mappings for the types actually passed to `Put` are required. The separate
`foo_only_tests.cpp` executable builds and links without a BarThing declaration.

- `Put(object)` extracts every property from an object. Every collected property
  must have an accessor specialization for the supplied object type.
- `Get<Property>()` returns a const reference to that property's `std::optional`
  result. Copy it if you need a snapshot before subsequent calls to `Put`.
- Before the first input, every result is empty. The first input establishes
  consensus, including values such as zero or an empty string.
- A disagreement clears only that property, permanently for that collector.
  Later matching values cannot restore it. Use a new collector for a new batch.

The collector compares results with `==`, so the example's prices use exact
floating-point equality. Define an appropriate result type with its own equality
if a property needs different comparison rules.
