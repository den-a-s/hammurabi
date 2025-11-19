#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>

constexpr int FINAL_ROUND = 10;
constexpr int MAX_PERCENT_DEAD_CITIZEN = 45;  // от голода

struct CityState {
  int32_t num_acre;
  int32_t num_citizen;
  int32_t bushels_wheat;
};

struct CityEvents {
  int32_t bushels_wheat_eaten_rats;
  int32_t num_citizen_dead;
  bool plague;
  int32_t bushels_wheat_per_acre;
  int32_t acre_price;
};

struct RulersDecisions {
  int32_t num_acre_to_buy;
  int32_t num_acre_to_sold;
  int32_t bushels_wheat_to_citizen;
  int32_t num_acre_to_plant;
};

struct RoundState {
  int32_t num_round;
  CityState city_state;
  std::optional<CityEvents> city_events;
  std::optional<RulersDecisions> rulers_decisions;
};

using GameState = std::vector<RoundState>;

GameState new_game_state() {
  RoundState default_parametrs{
      .num_round = 1,
      .city_state = CityState{.num_acre = 1000,
                              .num_citizen = 100,
                              .bushels_wheat = 2800},
      .city_events = std::nullopt,
      .rulers_decisions = std::nullopt};

  GameState gs;
  gs.reserve(10);
  gs.push_back(default_parametrs);
  return gs;
}

void menu() {
  std::cout << "Добро пожаловать в игру Хаммурапи: Правитель Египта."
            << std::endl;
  std::cout << "Начать игру:" << std::endl;
  std::cout << "1. Новая игра" << std::endl;
  std::cout << "2. Загрузить игру" << std::endl;
}

bool is_valid_num_round(int num_round) {
  return 1 <= num_round && num_round <= 10;
}

bool round_is_over(RoundState const& r) {
  return r.city_events && r.rulers_decisions;
}

bool is_final_round(RoundState const& r) { return r.num_round == FINAL_ROUND; }

/* Need to fill city_events.num_citizen_dead
                city_events.bushels_wheat_per_acre
                city_state.bushels_wheat */
int32_t get_new_citizen_num(CityState const& city_state,
                            CityEvents const& city_events) {
  auto new_citizen_num = city_events.num_citizen_dead / 2 *
                             (5 - city_events.bushels_wheat_per_acre) *
                             city_state.bushels_wheat / 600 + 1;
  return std::clamp(new_citizen_num, 0, 50);
}

CityEvents gen_new_city_events(int32_t const seed,
                               CityState const& city_state) {
  CityEvents city_events{ .bushels_wheat_eaten_rats = 0,
                          .num_citizen_dead = 0,
                          .plague = false,
                          .bushels_wheat_per_acre = 0,
                          .acre_price = 0};

  std::mt19937 gen(seed);

  std::uniform_int_distribution<> bushels_wheat_per_acre_distrib(1, 6);
  city_events.bushels_wheat_per_acre = bushels_wheat_per_acre_distrib(gen);

  std::uniform_real_distribution<> percent_bushels_eaten_rats_distrib(0, 0.07);
  int32_t bushels_wheat_with_harvested =
      city_state.bushels_wheat +
      city_events.bushels_wheat_per_acre * city_state.num_acre;
  city_events.bushels_wheat_eaten_rats =
      bushels_wheat_with_harvested * percent_bushels_eaten_rats_distrib(gen);

  constexpr int32_t bushels_wheat_per_citizen = 20;
  // Если есть остаток, то человек всё равно погибает с голоду
  int32_t num_citizen_can_be_fed =
      city_state.bushels_wheat / bushels_wheat_per_citizen;

  city_events.num_citizen_dead =
      std::max(0, city_state.num_citizen - num_citizen_can_be_fed);

  int32_t new_citizens = get_new_citizen_num(city_state, city_events);

  return city_events;
}

int main() {
  // UTF-8 позволяет кроссплатформенно выводить русские символы в консоль
  std::locale::global(std::locale("en_US.UTF-8"));

  int n;
  int num_round = 1;

  while (true) {
    menu();
    std::cin >> n;
    if (n == 1) {
      GameState state = new_game_state();
      while (state.size() < FINAL_ROUND) {
        auto current_round = state.back();

        if (!is_valid_num_round(current_round.num_round)) {
          std::cout << "Номер раунда не находится в диапозоне 1-10"
                    << std::endl;
          break;
        }

        if (round_is_over(current_round) && !is_final_round(current_round)) {
          std::cout << "Debug: Раунд уже сыгран" << std::endl;
          // Отрендерить сообщение
          // Добавить новый раунд в state
          continue;
        }

        
      }
    } else {
      std::cout << "Debug: Не реализовано" << std::endl;
      return 0;
    }
  }
  return 0;
}
