#pragma once
#include <cstdint>
#include <iostream>
#include <format>

#include "city_state.h"
#include "city_events.h"

struct RulersDecisions
{
  int num_acre_to_buy;
  int num_acre_to_sold;
  int bushels_wheat_to_citizen;
  int num_acre_to_plant;
};

RulersDecisions get_rulers_decisions(std::istream &in,
                                     CityState const &city_state,
                                     CityEvents const &city_events);

template <>
struct std::formatter<RulersDecisions>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const RulersDecisions &rs, std::format_context &ctx) const
  {
    return std::format_to(
        ctx.out(),
        "RulersDecisions[num_acre_to_buy={}, num_acre_to_sold={}, "
        "bushels_wheat_to_citizen={}, num_acre_to_plant={}]",
        rs.num_acre_to_buy, rs.num_acre_to_sold, rs.bushels_wheat_to_citizen,
        rs.num_acre_to_plant);
  }
};

json to_json(const RulersDecisions &decisions);

template <>
RulersDecisions from_json<RulersDecisions>(const json &j);
