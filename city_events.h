#pragma once
#include <cstdint>
#include <format>
#include "city_state.h"

struct CityEvents {
  int32_t bushels_wheat_eaten_rats;
  int32_t num_dead_citizen;
  int32_t num_new_citizen;
  bool plague;
  int32_t bushels_wheat_per_acre;
  int32_t acre_price;
};

CityEvents gen_new_city_events(int32_t const seed, CityState const& city_state);

/* Need to fill city_events.num_dead_citizen
                city_events.bushels_wheat_per_acre
                city_state.bushels_wheat */
int32_t get_new_citizen_num(CityState const& city_state,
                            CityEvents const& city_events);

int32_t get_num_cultivate_bushels(CityState const& city_state,
                                  CityEvents const& city_events);

template <>
struct std::formatter<CityEvents> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const CityEvents& rs, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "CityEvents[bushels_wheat_eaten_rats={}, num_dead_citizen={}, "
        "num_new_citizen={}, plague={}, bushels_wheat_per_acre={}, "
        "acre_price={}]",
        rs.bushels_wheat_eaten_rats, rs.num_dead_citizen, rs.num_new_citizen,
        rs.plague, rs.bushels_wheat_per_acre, rs.acre_price);
  }
};