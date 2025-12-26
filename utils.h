#pragma once

#include <cstdint>
#include <iostream>
#include <expected>
#include <vector>
#include <functional>
#include <format>
#include <json.hpp>

enum ValidationError
{
  ParseError,
  NegativeNumber,
  NotEnoughBushels,
  NotEnoughAcres,
  TooManyAcres,
  TooManyAcresToPlant,
};

template <>
struct std::formatter<ValidationError>
{
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const ValidationError &ve, std::format_context &ctx) const
  {
    std::string err_str;
    switch (ve)
    {
    case ValidationError::ParseError:
      err_str = "ParseError";
      break;
    case ValidationError::NegativeNumber:
      err_str = "NegativeNumber";
      break;
    case ValidationError::NotEnoughBushels:
      err_str = "NotEnoughBushels";
      break;
    case ValidationError::NotEnoughAcres:
      err_str = "NotEnoughAcres";
      break;
    case ValidationError::TooManyAcres:
      err_str = "TooManyAcres";
      break;
    case ValidationError::TooManyAcresToPlant:
      err_str = "TooManyAcresToPlant";
      break;
    }
    return std::format_to(ctx.out(), "{}", err_str);
  }
};

using ViewValidationError = std::function<void(ValidationError)>;
using Validator =
    std::function<std::expected<int, ValidationError>(int const &)>;

std::expected<int, ValidationError> validate_negative(
    int const &value);

std::expected<int, ValidationError> buf_to_int(const std::string &str);

int read_validated_input(
    std::istream &in, ViewValidationError error_view_func,
    std::vector<Validator> const &validators);

ViewValidationError print_validation_error();

using json = nlohmann::json;

template <typename T>
T from_json(const json &j) { static_assert(false, "Need specialisation"); }
