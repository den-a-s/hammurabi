#pragma once
#include <cstdint>
#include <format>
#include "utils.h"
#include "city_state.h"

struct CityEvents
{
  int bushels_wheat_eaten_rats;
  int num_dead_citizen;
  int num_new_citizen;
  bool plague;
  int bushels_wheat_per_acre;
  int acre_price;
};

CityEvents gen_new_city_events(int const seed, CityState const &city_state);

/* Need to fill city_events.num_dead_citizen
                city_events.bushels_wheat_per_acre
                city_state.bushels_wheat */
int get_new_citizen_num(CityState const &city_state,
                        CityEvents const &city_events);

int get_num_cultivate_bushels(CityState const &city_state,
                              CityEvents const &city_events);

template <>
struct std::formatter<CityEvents>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const CityEvents &rs, std::format_context &ctx) const
  {
    return std::format_to(
        ctx.out(),
        "CityEvents[bushels_wheat_eaten_rats={}, num_dead_citizen={}, "
        "num_new_citizen={}, plague={}, bushels_wheat_per_acre={}, "
        "acre_price={}]",
        rs.bushels_wheat_eaten_rats, rs.num_dead_citizen, rs.num_new_citizen,
        rs.plague, rs.bushels_wheat_per_acre, rs.acre_price);
  }
};

json to_json(const CityEvents &state);

template <>
CityEvents from_json<CityEvents>(const json &j);