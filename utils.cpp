#include "utils.h"

std::expected<int, ValidationError> validate_negative(
    int const &value)
{
    if (value > 0)
        return value;
    return std::unexpected(ValidationError::NegativeNumber);
}

std::expected<int, ValidationError> buf_to_int(const std::string &str)
{
    int value;
    auto result = std::from_chars(str.data(), str.data() + str.size(), value);

    if (result.ec == std::errc::invalid_argument)
    {
        return std::unexpected(ParseError);
    }
    if (result.ec == std::errc::result_out_of_range)
    {
        return std::unexpected(ParseError);
    }
    if (result.ptr != str.data() + str.size())
    {
        return std::unexpected(ParseError);
    }

    return value;
}

int read_validated_input(
    std::istream &in, ViewValidationError error_view_func,
    std::vector<Validator> const &validators)
{
    std::string buf;
    while (true)
    {
        in >> buf;
        auto res = buf_to_int(buf);
        if (!res.has_value())
        {
            error_view_func(ValidationError::ParseError);
            continue;
        }
        int value = res.value();
        bool is_error = false;
        for (auto &&valid : validators)
        {
            value = res.value();
            auto res_valid = std::invoke(valid, value);
            if (!res_valid.has_value())
            {
                error_view_func(res_valid.error());
                is_error = true;
            }
        }
        if (is_error)
            continue;

        return value;
    }
}

ViewValidationError print_validation_error()
{
    return [](ValidationError err)
    {
        std::cout << std::format("validation err: {}", err) << std::endl;
    };
}