#include "round_state.h"

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

  CityState const& city_state = prev_round.city_state;
  CityEvents const& city_events = prev_round.city_events.value();
  RulersDecisions const& rulers_decisions = prev_round.rulers_decisions.value();

 int32_t bushels_to_buy_acres =
      rulers_decisions.num_acre_to_buy * city_events.acre_price;

  int32_t bushels_to_sold_acres =
      rulers_decisions.num_acre_to_sold * city_events.acre_price;

  next_city_state.bushels_wheat =
      city_state.bushels_wheat +
      get_num_cultivate_bushels(city_state, city_events) -
      bushels_to_buy_acres + bushels_to_sold_acres;

  next_city_state.num_acre_with_wheat = rulers_decisions.num_acre_to_plant;

  next_city_state.num_citizen = city_state.num_citizen - city_events.num_dead_citizen + city_events.num_new_citizen;

  next_city_state.num_acre = city_state.num_acre +
                             rulers_decisions.num_acre_to_buy -
                             rulers_decisions.num_acre_to_sold;

  RoundState next_round{.num_round = prev_round.num_round + 1,
                        .city_state = next_city_state,
                        .city_events = std::nullopt,
                        .rulers_decisions = std::nullopt};

  return next_round;
}

double calc_mean_dead_persons(GameState const& game_state) {
  double sum_percent_dead_persons = 0.0;
  for (int i = 0; i < game_state.size() - 1;  i++) {
    auto all_count = game_state[i].city_state.num_citizen +
                     game_state[i].city_events->num_new_citizen -
                     game_state[i].city_events->num_dead_citizen;
    sum_percent_dead_persons +=
        game_state[i].city_events->num_dead_citizen / all_count;
  }
  return sum_percent_dead_persons / game_state.size() * 100;
}

double calc_num_acres_per_citizen(GameState const& game_state) {
  auto const& last_round = game_state[game_state.size() - 2];
  auto all_citizen = last_round.city_state.num_citizen +
                     last_round.city_events->num_new_citizen -
                     last_round.city_events->num_dead_citizen;

  auto all_acre = last_round.city_state.num_acre +
                  last_round.rulers_decisions->num_acre_to_buy -
                  last_round.rulers_decisions->num_acre_to_sold;
  return all_acre / all_citizen;
}
