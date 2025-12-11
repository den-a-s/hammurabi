#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <format>

#include "city_state.h"
#include "city_events.h"
#include "rullers_decisions.h"

constexpr int FINAL_ROUND = 2;
constexpr int MAX_PERCENT_DEAD_CITIZEN = 45;  // от голода

struct RoundState {
  int32_t num_round;
  CityState city_state;
  std::optional<CityEvents> city_events;
  std::optional<RulersDecisions> rulers_decisions;
};

using GameState = std::vector<RoundState>;

GameState new_game_state();

bool is_valid_round(RoundState const& r);

bool round_is_over(RoundState const& r);

bool is_final_round(RoundState const& r);

RoundState get_next_round(RoundState const& prev_round);

double calc_mean_dead_persons(GameState const& game_state);

double calc_num_acres_per_citizen(GameState const& game_state);

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