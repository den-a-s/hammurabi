#include "round_state.h"

bool is_valid_round(RoundState const& r) {
  return 1 <= r.num_round && r.num_round <= 10;
}

bool round_is_over(RoundState const& r) {
  return r.city_events && r.rulers_decisions;
}

bool is_final_round(RoundState const& r) { return r.num_round == FINAL_ROUND; }

RoundState get_next_round(RoundState const& prev_round) {
  CityState next_city_state{
      .num_acre = 0,
      .num_acre_with_wheat = 0,
      .num_citizen = 0,
      .bushels_wheat = 0,
  };

  next_city_state.num_acre = prev_round.city_state.num_acre +
                             prev_round.rulers_decisions->num_acre_to_buy +
                             prev_round.rulers_decisions->num_acre_to_sold;

  RoundState next_round{.num_round = prev_round.num_round + 1,
                        .city_state = CityState{.num_acre = 1000,
                                                .num_citizen = 100,
                                                .bushels_wheat = 2800},
                        .city_events = std::nullopt,
                        .rulers_decisions = std::nullopt};

  return next_round;
}
