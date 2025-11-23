#include "rullers_decisions.h"

#include "utils.h"

#include <expected>

// TODO возвращать std::expected<int32_t, Enum error>
// где enum error содержит в себе все ошибки не правильного парсинга и
// логические ошибки чтобы можно было выше решить какое сообщение вывести
// пользователю
int32_t read_num_acre_to_buy_with_retry(std::istream& in,
                                        CityState const& city_state,
                                        CityEvents const& city_events) {
  return read_int_with_retry(
      in,
      [&city_state, &city_events](
          std::string const& buf) -> std::expected<int32_t, std::string> {
        auto res = buf_to_int(buf);
        if (!res.has_value()) return res;

        int32_t num_acre_to_buy = res.value();

        if (num_acre_to_buy < 0) {
          return std::unexpected("negative num");
        }

        int32_t bushels_wheat_with_harvested =
            city_state.bushels_wheat +
            get_num_cultivate_bushels(city_state, city_events);

        int32_t bushels_acre_buy = num_acre_to_buy * city_events.acre_price;

        // TODO понять с собранными ли копнами мы считаем или нет
        bool is_can_buy_acre = bushels_acre_buy <= city_state.bushels_wheat;

        if (!is_can_buy_acre) {
          return std::unexpected("You cant buy bushels");
        }

        return num_acre_to_buy;
      });
}

// TODO возвращать std::expected<int32_t, Enum error>
// где enum error содержит в себе все ошибки не правильного парсинга и
// логические ошибки чтобы можно было выше решить какое сообщение вывести
// пользователю
int32_t read_num_acre_to_sold_with_retry(std::istream& in,
                                         CityState const& city_state,
                                         CityEvents const& city_events) {
  return read_int_with_retry(
      in,
      [&city_state, &city_events](
          std::string const& buf) -> std::expected<int32_t, std::string> {
        auto res = buf_to_int(buf);
        if (!res.has_value()) return res;

        int32_t num_acre_to_sold = res.value();

        if (num_acre_to_sold < 0) {
          return std::unexpected("negative num");
        }

        bool is_can_sold_acres = num_acre_to_sold <= city_state.num_acre;

        if (!is_can_sold_acres) {
          return std::unexpected("You cant sold acre");
        }

        return num_acre_to_sold;
      });
}

// TODO возвращать std::expected<int32_t, Enum error>
// где enum error содержит в себе все ошибки не правильного парсинга и
// логические ошибки чтобы можно было выше решить какое сообщение вывести
// пользователю
int32_t read_bushels_wheat_to_citizen_with_retry(
    std::istream& in, CityState const& city_state,
    CityEvents const& city_events, RulersDecisions const& rulers_decisions) {
  return read_int_with_retry(
      in,
      [&city_state, &city_events, &rulers_decisions](
          std::string const& buf) -> std::expected<int32_t, std::string> {
        auto res = buf_to_int(buf);
        if (!res.has_value()) return res;

        int32_t bushels_wheat_to_citizen = res.value();

        if (bushels_wheat_to_citizen < 0) {
          return std::unexpected("negative num");
        }

        int32_t bushels_to_buy_acres =
            rulers_decisions.num_acre_to_buy * city_events.acre_price;

        int32_t bushels_to_sold_acres =
            rulers_decisions.num_acre_to_sold * city_events.acre_price;

        int32_t bushels_wheat_after_rulers_decision =
            city_state.bushels_wheat +
            get_num_cultivate_bushels(city_state, city_events) -
            bushels_to_buy_acres + bushels_to_sold_acres;

        bool is_can_give_bushels =
            bushels_wheat_to_citizen <= bushels_wheat_after_rulers_decision;

        if (!is_can_give_bushels) {
          return std::unexpected("You cant give bushels");
        }

        return bushels_wheat_to_citizen;
      });
}

// TODO возвращать std::expected<int32_t, Enum error>
// где enum error содержит в себе все ошибки не правильного парсинга и
// логические ошибки чтобы можно было выше решить какое сообщение вывести
// пользователю
int32_t read_num_acre_to_plant_with_retry(
    std::istream& in, CityState const& city_state,
    CityEvents const& city_events, RulersDecisions const& rulers_decisions) {
  return read_int_with_retry(
      in,
      [&city_state, &city_events, &rulers_decisions](
          std::string const& buf) -> std::expected<int32_t, std::string> {
        auto res = buf_to_int(buf);
        if (!res.has_value()) return res;

        int32_t num_acre_to_plant = res.value();

        if (num_acre_to_plant < 0) {
          return std::unexpected("negative num");
        }

        int32_t acres_after_rulers_decision = city_state.num_acre -
                                              rulers_decisions.num_acre_to_buy +
                                              rulers_decisions.num_acre_to_sold;

        bool is_can_plant_bushels =
            num_acre_to_plant <= acres_after_rulers_decision;

        if (!is_can_plant_bushels) {
          return std::unexpected("You cant plant bushels");
        }

        return num_acre_to_plant;
      });
}

RulersDecisions get_rulers_decisions(std::istream& in,
                                     CityState const& city_state,
                                     CityEvents const& city_events) {
  RulersDecisions rd{
      .num_acre_to_buy = 0,
      .num_acre_to_sold = 0,
      .bushels_wheat_to_citizen = 0,
      .num_acre_to_plant = 0,
  };

  rd.num_acre_to_buy = read_num_acre_to_buy_with_retry(in, city_state, city_events);
  rd.num_acre_to_sold =
      read_num_acre_to_sold_with_retry(in, city_state, city_events);
  rd.bushels_wheat_to_citizen =
      read_bushels_wheat_to_citizen_with_retry(in, city_state, city_events, rd);
  rd.num_acre_to_plant =
      read_num_acre_to_plant_with_retry(in, city_state, city_events, rd);

  return rd;
}
