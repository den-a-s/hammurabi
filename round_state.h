#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <format>

#include "city_state.h"
#include "city_events.h"
#include "rullers_decisions.h"

constexpr int FINAL_ROUND = 10;
constexpr int MAX_PERCENT_DEAD_CITIZEN = 45;  // от голода

struct RoundState {
  int32_t num_round;
  CityState city_state;
  std::optional<CityEvents> city_events;
  std::optional<RulersDecisions> rulers_decisions;
};

using GameState = std::vector<RoundState>;

GameState new_game_state() {
  // —читаем что изначально 1000 акров засажено пшеницей
  RoundState default_parametrs{
      .num_round = 1,
      .city_state = CityState{.num_acre = 1000,
                              .num_acre_with_wheat = 1000,
                              .num_citizen = 100,
                              .bushels_wheat = 2800},
      .city_events = std::nullopt,
      .rulers_decisions = std::nullopt};

  GameState gs;
  gs.reserve(10);
  gs.push_back(default_parametrs);
  return gs;
}

bool is_valid_round(RoundState const& r);

bool round_is_over(RoundState const& r);

bool is_final_round(RoundState const& r);

RoundState get_next_round(RoundState const& prev_round);

template <>
struct std::formatter<RoundState> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const RoundState& rs, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "RoundState[num_round={}, city_state={}, "
        "city_events={}, rulers_decisions={}]",
        rs.num_round, rs.city_state,
        rs.city_events ? std::format("{}", *rs.city_events) : "none",
        rs.rulers_decisions ? std::format("{}", *rs.city_events) : "none");
  }
};