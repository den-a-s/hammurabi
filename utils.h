#pragma once

#include <cstdint>
#include <iostream>
#include <expected>

template <typename F>
int32_t read_int_with_retry(std::istream& in, F validate) {
  std::string buf;

  while (true) {
    in >> buf;
    auto res = validate(buf);
    if (res.has_value()) {
      return res.value();
    }
    std::cout << res.error();
  }
}

std::expected<int32_t, std::string> buf_to_int(const std::string& str) {
  int32_t value;
  auto result = std::from_chars(str.data(), str.data() + str.size(), value);

  if (result.ec == std::errc::invalid_argument) {
    return std::unexpected("Invalid argument");
  }
  if (result.ec == std::errc::result_out_of_range) {
    return std::unexpected("Out of range");
  }
  if (result.ptr != str.data() + str.size()) {
    return std::unexpected("Extra characters after number");
  }

  return value;
}