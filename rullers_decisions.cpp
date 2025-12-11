#include "rullers_decisions.h"

#include <print>

#include <expected>

#include "utils.h"

Validator validate_num_acre_to_buy(CityState const& city_state,
                                   CityEvents const& city_events) {
  return [&city_state, &city_events](
             int32_t const& value) -> std::expected<int32_t, ValidationError> {
    int32_t bushels_wheat_with_harvested =
        city_state.bushels_wheat +
        get_num_cultivate_bushels(city_state, city_events);

    int32_t bushels_acre_buy = value * city_events.acre_price;

    // TODO понять с собранными ли копнами мы считаем или нет
    bool is_can_buy_acre = bushels_acre_buy <= city_state.bushels_wheat;

    if (!is_can_buy_acre) {
      return std::unexpected(ValidationError::NotEnoughAcres);
    }

    return value;
  };
}

Validator validate_num_acre_to_sold(CityState const& city_state,
                                    CityEvents const& city_events) {
  return [&city_state, &city_events](
             int32_t const& value) -> std::expected<int32_t, ValidationError> {
    bool is_can_sold_acres = value <= city_state.num_acre;

    if (!is_can_sold_acres) {
      return std::unexpected(ValidationError::TooManyAcres);
    }

    return value;
  };
}

Validator validate_bushels_wheat_to_citizen(
    CityState const& city_state, CityEvents const& city_events,
    RulersDecisions const& rulers_decisions) {
  return [&city_state, &city_events,
          &rulers_decisions](int32_t const& bushels_wheat_to_citizen)
             -> std::expected<int32_t, ValidationError> {
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
      return std::unexpected(ValidationError::NotEnoughBushels);
    }

    return bushels_wheat_to_citizen;
  };
}

Validator validate_num_acre_to_plant(CityState const& city_state,
                                     CityEvents const& city_events,
                                     RulersDecisions const& rulers_decisions) {
  return [&city_state, &city_events,
          &rulers_decisions](int32_t const& num_acre_to_plant)
             -> std::expected<int32_t, ValidationError> {
    int32_t acres_after_rulers_decision = city_state.num_acre -
                                          rulers_decisions.num_acre_to_buy +
                                          rulers_decisions.num_acre_to_sold;

    bool is_can_plant_bushels =
        num_acre_to_plant <= acres_after_rulers_decision;

    if (!is_can_plant_bushels) {
      return std::unexpected(ValidationError::TooManyAcresToPlant);
    }

    return num_acre_to_plant;
  };
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

  auto view_validation_error = print_validation_error();
  
  // Блок с чтением и валидацией покупаемых акров земли
  std::cout << "Сколько акров повелеваешь купить?" << std::endl;

  rd.num_acre_to_buy = read_validated_input(
      in, view_validation_error,
      std::vector<Validator>{
          {validate_negative,
           validate_num_acre_to_buy(city_state, city_events)}});

  // Блок с чтением и валидацией продаваемых акров земли
  std::cout << "Сколько акров повелеваешь продать?" << std::endl;

  rd.num_acre_to_sold = read_validated_input(
      in, view_validation_error,
      std::vector<Validator>{
          {validate_negative,
           validate_num_acre_to_sold(city_state, city_events)}});

  // Блок с чтением и валидацией съедаемых запасов пшеницы
  std::cout << "Сколько бушелей пшеницы повелеваешь съесть?"<< std::endl;

  rd.bushels_wheat_to_citizen = read_validated_input(
      in, view_validation_error,
      std::vector<Validator>{
          {validate_negative,
           validate_bushels_wheat_to_citizen(city_state, city_events, rd)}});

  // Блок с чтением и валидацией акров которые отдаются под засев
  std::cout << "Сколько акров земли повелеваешь засеять?" << std::endl;

  rd.num_acre_to_plant = read_validated_input(
      in, view_validation_error,
      std::vector<Validator>{
          {validate_negative,
           validate_num_acre_to_plant(city_state, city_events, rd)}});

  return rd;
}
