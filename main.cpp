#include <iostream>
#include <cstdint>
#include <optional>

struct CityState {
  int16_t num_acre;
  int16_t num_citizen;
  int16_t bushels_wheat;
};

struct CityEvents {
  int16_t bushels_wheat_eaten_rats;
  int16_t num_citizen_dead;
  bool    plague;
  int16_t bushels_wheat_per_acre;
  int16_t acre_price;
};

struct RulersDecisions {
  int16_t num_acre_to_buy;
  int16_t num_acre_to_sold;
  int16_t bushels_wheat_to_citizen;
  int16_t num_acre_to_plant;
};

int main() {
  int n;
  while(true) {
    std::cout << "Привет, мир!" << std::endl;
    std::cin >> n;
    if( n == 0 )
      break;
  }
  return 0;
}
