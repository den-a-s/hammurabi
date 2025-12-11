#pragma once

#include <cstdint>
#include <iostream>
#include <expected>
#include <vector>
#include <functional>

enum ValidationError {
  ParseError,
  NegativeNumber,
  NotEnoughBushels,
  NotEnoughAcres,
  TooManyAcres,
  TooManyAcresToPlant,
};

// Специализация formatter для ValidationError
template <>
struct std::formatter<ValidationError> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const ValidationError& ve, std::format_context& ctx) const {
    std::string err_str;
    switch (ve) {
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
    std::function<std::expected<int32_t, ValidationError>(int32_t const&)>;

std::expected<int32_t, ValidationError> validate_negative(
    int32_t const& value) {
  if (value > 0) return value;
  return std::unexpected(ValidationError::NegativeNumber);
}

std::expected<int32_t, ValidationError> buf_to_int(const std::string& str) {
  int32_t value;
  auto result = std::from_chars(str.data(), str.data() + str.size(), value);

  if (result.ec == std::errc::invalid_argument) {
    return std::unexpected(ParseError);
  }
  if (result.ec == std::errc::result_out_of_range) {
    return std::unexpected(ParseError);
  }
  if (result.ptr != str.data() + str.size()) {
    return std::unexpected(ParseError);
  }

  return value;
}

int32_t read_validated_input(
    std::istream& in, ViewValidationError error_view_func,
    std::vector<Validator> const& validators) {
  std::string buf;
  while (true) {
    in >> buf;
    auto res = buf_to_int(buf);
    if (!res.has_value()) {
       error_view_func(ValidationError::ParseError);
    }
    int32_t value = res.value();
    for (auto&& valid : validators) {
      value = res.value();
      auto res_valid = std::invoke(valid, value);
      if (!res_valid.has_value()) {
        error_view_func(res_valid.error());
      }
    }

    return value;
  }
}

ViewValidationError print_validation_error() {
  return [](ValidationError err) {
    std::cout << std::format("validation err: {}",err) << std::endl;
  };
}