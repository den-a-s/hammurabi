#include "city_events.h"

#include <random>
#include <algorithm>

CityEvents gen_new_city_events(int const seed,
                               CityState const &city_state)
{
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
    int bushels_wheat_with_harvested =
        city_state.bushels_wheat +
        get_num_cultivate_bushels(city_state, city_events);

    city_events.bushels_wheat_eaten_rats =
        bushels_wheat_with_harvested * percent_bushels_eaten_rats_distrib(gen);

    constexpr int bushels_wheat_per_citizen = 20;

    int can_be_fed_num_citizen =
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
int get_new_citizen_num(CityState const &city_state,
                        CityEvents const &city_events)
{
    auto new_citizen_num = city_events.num_dead_citizen / 2 *
                               (5 - city_events.bushels_wheat_per_acre) *
                               city_state.bushels_wheat / 600 +
                           1;
    return std::clamp(new_citizen_num, 0, 50);
}

int get_num_cultivate_bushels(CityState const &city_state,
                              CityEvents const &city_events)
{
    constexpr int num_acre_cultivated_one_citizen = 10;
    const int num_can_cultivate_acre =
        city_state.num_citizen * num_acre_cultivated_one_citizen;

    const int num_cultivate_acre =
        std::clamp(num_can_cultivate_acre, 0, city_state.num_acre_with_wheat);

    return num_cultivate_acre * city_events.bushels_wheat_per_acre;
}

json to_json(const CityEvents &events)
{
    return json{
        {"bushels_wheat_eaten_rats", events.bushels_wheat_eaten_rats},
        {"num_dead_citizen", events.num_dead_citizen},
        {"num_new_citizen", events.num_new_citizen},
        {"plague", events.plague},
        {"bushels_wheat_per_acre", events.bushels_wheat_per_acre},
        {"acre_price", events.acre_price}};
}

template <>
CityEvents from_json<CityEvents>(const json &j)
{
    CityEvents events;
    j.at("bushels_wheat_eaten_rats").get_to(events.bushels_wheat_eaten_rats);
    j.at("num_dead_citizen").get_to(events.num_dead_citizen);
    j.at("num_new_citizen").get_to(events.num_new_citizen);
    j.at("plague").get_to(events.plague);
    j.at("bushels_wheat_per_acre").get_to(events.bushels_wheat_per_acre);
    j.at("acre_price").get_to(events.acre_price);
    return events;
}
