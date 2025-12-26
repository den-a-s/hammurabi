#pragma once
#include <cstdint>
#include <format>
#include <json.hpp>
#include "utils.h"

using json = nlohmann::json;

struct CityState
{
  int num_acre;
  int num_acre_with_wheat;
  int num_citizen;
  int bushels_wheat;
};

template <>
struct std::formatter<CityState>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const CityState &v, std::format_context &ctx) const
  {
    return std::format_to(ctx.out(),
                          "CityState[num_acre={}, num_acre_with_wheat={}, "
                          "num_citizen={}, bushels_wheat={}]",
                          v.num_acre, v.num_acre_with_wheat, v.num_citizen,
                          v.bushels_wheat);
  }
};

json to_json(const CityState &state);

template <>
CityState from_json<CityState>(const json &j);
