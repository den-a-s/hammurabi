#include "round_state.h"

#include "city_state.h"

#include <random>
#include <iostream>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;

static const std::string SAVE_FILE_PATH = SAVE_FILE;

static int generate_seed()
{
#ifdef SEED
  return SEED;
#else
  std::random_device rd;
  std::uniform_int_distribution<int> dist;
  return dist(rd);
#endif
}

json to_json(const RoundState &state)
{
  json j{{"num_round", state.num_round}};

  j["city_state"] = to_json(state.city_state);

  if (state.city_events.has_value())
  {
    j["city_events"] = to_json(state.city_events.value());
  }
  if (state.rulers_decisions.has_value())
  {
    j["ruler_decisions"] = to_json(state.rulers_decisions.value());
  }
  return j;
}

template <>
RoundState from_json<RoundState>(const json &j)
{
  RoundState state;
  j.at("num_round").get_to(state.num_round);
  state.city_state = from_json<CityState>(j.at("city_state"));

  // Для optional полей проверяем наличие в JSON
  if (j.contains("city_events"))
  {
    state.city_events = from_json<CityEvents>(j.at("city_events"));
  }

  if (j.contains("ruler_decisions"))
  {
    state.rulers_decisions = from_json<RulersDecisions>(j.at("ruler_decisions"));
  }
  return state;
}

json to_json(const GameState &state)
{
  json j{{"seed", state.seed}};

  json rounds = json::array();
  for (const auto &item : state.rounds)
  {
    rounds.push_back(to_json(item));
  }
  j["rounds"] = rounds;

  return j;
}

template <>
GameState from_json<GameState>(const json &j)
{
  GameState gs;
  j.at("seed").get_to(gs.seed);

  json arr = j.at("rounds");
  for (const auto &item : arr)
  {
    gs.rounds.push_back(from_json<RoundState>(item));
  }

  return gs;
}

static GameState default_game_state()
{
  // Задаём по умолчанию
  RoundState default_parametrs{
      .num_round = 1,
      .city_state = CityState{.num_acre = 1000,
                              .num_acre_with_wheat = 1000,
                              .num_citizen = 100,
                              .bushels_wheat = 2800},
      .city_events = std::nullopt,
      .rulers_decisions = std::nullopt};

  GameState gs{
      .seed = generate_seed(),
      .rounds = {{default_parametrs}}};
  return gs;
}

GameState start_game_state(StartGameState cmd)
{
  if (cmd == StartGameState::LoadGame)
  {
    std::ifstream save_file(SAVE_FILE_PATH);

    if (!save_file.is_open())
    {
      std::cout << "Не найдено сохранения, начинаем новую игру" << std::endl;
      return default_game_state();
    }

    try
    {
      json data = json::parse(save_file);

      GameState gs = from_json<GameState>(data);

      std::cout << "Игра загружена из сохранения" << std::endl;

      return gs;
    }
    catch (const json::exception &e)
    {
      std::cerr << "Ошибка чтения сохранения: " << e.what() << std::endl;
      throw;
    }
  }

  return default_game_state();
}

void save_game(GameState const &game_state)
{
  std::ofstream save_file(SAVE_FILE_PATH);

  try
  {
    json data = to_json(game_state);

    save_file << data;

    std::cout << "Игра сохранена" << std::endl;
  }
  catch (const json::exception &e)
  {
    std::cerr << "Ошибка сохранения: " << e.what() << std::endl;
    throw;
  }
}

bool is_valid_round(RoundState const &r)
{
  return 1 <= r.num_round && r.num_round <= 10;
}

bool round_is_over(RoundState const &r)
{
  return r.city_events && r.rulers_decisions;
}

bool is_final_round(RoundState const &r) { return r.num_round == FINAL_ROUND; }

RoundState get_next_round(RoundState const &prev_round)
{
  CityState next_city_state{
      .num_acre = 0,
      .num_acre_with_wheat = 0,
      .num_citizen = 0,
      .bushels_wheat = 0,
  };

  CityState const &city_state = prev_round.city_state;
  CityEvents const &city_events = prev_round.city_events.value();
  RulersDecisions const &rulers_decisions = prev_round.rulers_decisions.value();

  int bushels_to_buy_acres =
      rulers_decisions.num_acre_to_buy * city_events.acre_price;

  int bushels_to_sold_acres =
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

double calc_mean_dead_persons(std::vector<RoundState> const &game_state)
{
  double sum_percent_dead_persons = 0.0;
  for (int i = 0; i < game_state.size() - 1; i++)
  {
    auto all_count = game_state[i].city_state.num_citizen +
                     game_state[i].city_events->num_new_citizen -
                     game_state[i].city_events->num_dead_citizen;
    sum_percent_dead_persons +=
        game_state[i].city_events->num_dead_citizen / all_count;
  }
  return sum_percent_dead_persons / game_state.size() * 100;
}

double calc_num_acres_per_citizen(std::vector<RoundState> const &game_state)
{
  auto const &last_round = game_state[game_state.size() - 2];
  auto all_citizen = last_round.city_state.num_citizen +
                     last_round.city_events->num_new_citizen -
                     last_round.city_events->num_dead_citizen;

  auto all_acre = last_round.city_state.num_acre +
                  last_round.rulers_decisions->num_acre_to_buy -
                  last_round.rulers_decisions->num_acre_to_sold;
  return all_acre / all_citizen;
}
