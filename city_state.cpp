#include "city_state.h"

json to_json(const CityState &state)
{
    return json{
        {"num_acre", state.num_acre},
        {"num_acre_with_wheat", state.num_acre_with_wheat},
        {"num_citizen", state.num_citizen},
        {"bushels_wheat", state.bushels_wheat}};
}

template <>
CityState from_json<CityState>(const json &j)
{
    CityState state;
    j.at("num_acre").get_to(state.num_acre);
    j.at("num_acre_with_wheat").get_to(state.num_acre_with_wheat);
    j.at("num_citizen").get_to(state.num_citizen);
    j.at("bushels_wheat").get_to(state.bushels_wheat);
    return state;
}
