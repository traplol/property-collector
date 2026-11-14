#pragma once

#include <optional>
#include <tuple>
#include <utility>

namespace accessor
{
// Property identifiers describe results independently of source types.
template<typename ResultT>
struct Property
{
  using result_t = ResultT;
};

// Define each source-to-property mapping alongside that source type.
template<typename PropertyT, typename ValueT>
struct Accessor;
}

namespace collector
{

template<typename ...Properties>
class Collector
{
  template<typename PropertyT>
  struct PropertyState
  {
    bool seen = false;
    std::optional<typename PropertyT::result_t> consensus;

    template<typename Value>
    void Put(const Value& value)
    {
      if (seen && !consensus) {
        return;
      }

      typename PropertyT::result_t result = accessor::Accessor<PropertyT, Value>{}(value);
      if (!seen) {
        consensus = std::move(result);
        seen = true;
      }
      else if (!(*consensus == result)) {
        consensus.reset();
      }
    }
  };

public:
  template<typename PropertyT>
  const std::optional<typename PropertyT::result_t>& Get() const
  {
    return std::get<PropertyState<PropertyT>>(m_properties).consensus;
  }

  template<typename Value>
  void Put(const Value& value)
  {
    std::apply([&value](auto&... properties) {
      (properties.Put(value), ...);
    }, m_properties);
  }

private:
  std::tuple<PropertyState<Properties>...> m_properties;
};

}
