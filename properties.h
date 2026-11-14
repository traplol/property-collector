#pragma once

#include "collector.h"
#include <string>

namespace property
{
struct Name : accessor::Property<std::string> {};
struct Price : accessor::Property<double> {};
struct Quantity : accessor::Property<int> {};
}
