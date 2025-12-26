#pragma once

#include <vector>
#include <cstdint>
#include <optional>
#include <format>

#include "city_state.h"
#include "city_events.h"
#include "rullers_decisions.h"

constexpr int FICTIVE_ROUND = 1;
constexpr int FINAL_ROUND = 10 + FICTIVE_ROUND;
constexpr int MAX_PERCENT_DEAD_CITIZEN = 45;

enum class StartGameState
{
  NewGame = 1,
  LoadGame = 2
};

struct RoundState
{
  int num_round;
  CityState city_state;
  std::optional<CityEvents> city_events;
  std::optional<RulersDecisions> rulers_decisions;
};

struct GameState
{
  int seed;
  std::vector<RoundState> rounds;
};

GameState start_game_state(StartGameState cmd);

void save_game(GameState const &game_state);

bool is_valid_round(RoundState const &r);

bool round_is_over(RoundState const &r);

bool is_final_round(RoundState const &r);

RoundState get_next_round(RoundState const &prev_round);

double calc_mean_dead_persons(std::vector<RoundState> const &game_state);

double calc_num_acres_per_citizen(std::vector<RoundState> const &game_state);

template <>
struct std::formatter<RoundState>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const RoundState &rs, std::format_context &ctx) const
  {
    return std::format_to(
        ctx.out(),
        "RoundState[num_round={}, city_state={}, "
        "city_events={}, rulers_decisions={}]",
        rs.num_round, rs.city_state,
        rs.city_events ? std::format("{}", *rs.city_events) : "none",
        rs.rulers_decisions ? std::format("{}", *rs.city_events) : "none");
  }
};

json to_json(const RoundState &state);

template <>
RoundState from_json<RoundState>(const json &j);

json to_json(const GameState &state);

template <>
GameState from_json<GameState>(const json &j);
