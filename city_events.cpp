#include "city_events.h"

#include <random>

CityEvents gen_new_city_events(int32_t const seed,
                               CityState const& city_state) {
  CityEvents city_events{.bushels_wheat_eaten_rats = 0,
                         .num_dead_citizen = 0,
                         .num_new_citizen = 0,
                         .plague = false,
                         .bushels_wheat_per_acre = 0,
                         .acre_price = 0};

  std::mt19937 gen(seed);

  std::uniform_int_distribution<> bushels_wheat_per_acre_distrib(1, 6);
  city_events.bushels_wheat_per_acre = bushels_wheat_per_acre_distrib(gen);

  std::uniform_real_distribution<> percent_bushels_eaten_rats_distrib(0, 0.07);
  int32_t bushels_wheat_with_harvested =
      city_state.bushels_wheat +
      get_num_cultivate_bushels(city_state, city_events);

  city_events.bushels_wheat_eaten_rats =
      bushels_wheat_with_harvested * percent_bushels_eaten_rats_distrib(gen);

  constexpr int32_t bushels_wheat_per_citizen = 20;
  // Если есть остаток, то человек всё равно погибает с голоду
  int32_t can_be_fed_num_citizen =
      city_state.bushels_wheat / bushels_wheat_per_citizen;

  city_events.num_dead_citizen =
      std::max(0, city_state.num_citizen - can_be_fed_num_citizen);

  city_events.num_new_citizen = get_new_citizen_num(city_state, city_events);

  std::uniform_real_distribution<> plague_distrib{};
  double plauge_chance_percent = plague_distrib(gen) * 100;
  city_events.plague = plauge_chance_percent <= 15;

  std::uniform_int_distribution<> acre_price_distrib(17, 26);
  city_events.acre_price = acre_price_distrib(gen);

  return city_events;
}

/* Need to fill city_events.num_dead_citizen
                city_events.bushels_wheat_per_acre
                city_state.bushels_wheat */
int32_t get_new_citizen_num(CityState const& city_state,
                            CityEvents const& city_events) {
  auto new_citizen_num = city_events.num_dead_citizen / 2 *
                             (5 - city_events.bushels_wheat_per_acre) *
                             city_state.bushels_wheat / 600 +
                         1;
  return std::clamp(new_citizen_num, 0, 50);
}

int32_t get_num_cultivate_bushels(CityState const& city_state,
                                  CityEvents const& city_events) {
  constexpr int32_t num_acre_cultivated_one_citizen = 10;
  const int32_t num_can_cultivate_acre =
      city_state.num_citizen * num_acre_cultivated_one_citizen;

  const int32_t num_cultivate_acre =
      std::clamp(num_can_cultivate_acre, 0, city_state.num_acre_with_wheat);

  return num_cultivate_acre * city_events.bushels_wheat_per_acre;
}