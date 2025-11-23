#pragma once
#include <cstdint>
#include <format>

struct CityState {
  int32_t num_acre;
  int32_t num_acre_with_wheat;
  int32_t num_citizen;
  int32_t bushels_wheat;
};

template <>
struct std::formatter<CityState> {
  constexpr auto parse(std::format_parse_context& ctx);

  auto format(const CityState& v, std::format_context& ctx) const;
};

inline constexpr auto std::formatter<CityState>::parse(
    std::format_parse_context& ctx) {
  return ctx.begin();
}

inline auto std::formatter<CityState>::format(const CityState& v,
                                              std::format_context& ctx) const {
  return std::format_to(ctx.out(),
                        "CityState[num_acre={}, num_acre_with_wheat={}, "
                        "num_citizen={}, bushels_wheat={}]",
                        v.num_acre, v.num_acre_with_wheat, v.num_citizen,
                        v.bushels_wheat);
}
