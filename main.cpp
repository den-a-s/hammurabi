#include <algorithm>
#include <charconv>
#include <cstdint>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <system_error>
#include <string_view>

constexpr int FINAL_ROUND = 10;
constexpr int MAX_PERCENT_DEAD_CITIZEN = 45;  // от голода

struct CityState {
  int32_t num_acre;
  int32_t num_acre_with_wheat;
  int32_t num_citizen;
  int32_t bushels_wheat;
};

struct CityEvents {
  int32_t bushels_wheat_eaten_rats;
  int32_t num_dead_citizen;
  int32_t num_new_citizen;
  bool plague;
  int32_t bushels_wheat_per_acre;
  int32_t acre_price;
};

struct RulersDecisions {
  int32_t num_acre_to_buy;
  int32_t num_acre_to_sold;
  int32_t bushels_wheat_to_citizen;
  int32_t num_acre_to_plant;
};

struct RoundState {
  int32_t num_round;
  CityState city_state;
  std::optional<CityEvents> city_events;
  std::optional<RulersDecisions> rulers_decisions;
};

using GameState = std::vector<RoundState>;

static std::expected<int32_t, std::string> buf_to_int(const std::string& str) {
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

GameState new_game_state() {
  // Считаем что изначально 1000 акров засажено пшеницей
  RoundState default_parametrs{
      .num_round = 1,
      .city_state = CityState{.num_acre = 1000,
                              .num_acre_with_wheat = 1000,
                              .num_citizen = 100,
                              .bushels_wheat = 2800},
      .city_events = std::nullopt,
      .rulers_decisions = std::nullopt};

  GameState gs;
  gs.reserve(10);
  gs.push_back(default_parametrs);
  return gs;
}

void menu() {
  std::cout << "Добро пожаловать в игру Хаммурапи: Правитель Египта."
            << std::endl;
  std::cout << "Начать игру:" << std::endl;
  std::cout << "1. Новая игра" << std::endl;
  std::cout << "2. Загрузить игру" << std::endl;
}

bool is_valid_num_round(int num_round) {
  return 1 <= num_round && num_round <= 10;
}

bool round_is_over(RoundState const& r) {
  return r.city_events && r.rulers_decisions;
}

bool is_final_round(RoundState const& r) { return r.num_round == FINAL_ROUND; }

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

void print_message_to_ruler(RoundState const& rs) {
    // год
    // голод, прибыли
    // чума
    // население сейчас
    // собрали пшеницы с пшеницей с акра
    // крысы истребили, оставили в амбарах
    // сколько акров занимает город
    // стоимость акра
    const std::string tmpl = "Мой повелитель, соизволь поведать тебе "
        "в году {} твоего высочайшего правления\n"
        "{}, и {};\n"
        "{}\n"
        "{}\n"
        "{}\n"
        "{}\n"
        "{}\n"
        "{}\n"
        "Что пожелаешь, повелитель?\n";

    CityState const& cs = rs.city_state;
    CityEvents const& ce = rs.city_events.value();

    std::string dead_citizen_str;
    if (ce.num_dead_citizen > 0) {
      dead_citizen_str = std::format("{} человек умерли с голоду,", ce.num_dead_citizen);
    }
    else
    {
      dead_citizen_str = "никто не умер с голоду";
    }

    std::string new_citizen_str;
    if (ce.num_new_citizen > 0) {
      new_citizen_str =
          std::format("{} человек прибыли в наш великий город", ce.num_new_citizen);
    } else {
      dead_citizen_str = "никто не прибыл в наш город";
    }

    std::string plague_str;
    if (ce.plague) {
      plague_str = "Чума уничтожила половину населения;";
    }

    std::string num_citizen_str =
        std::format("Население города сейчас составляет {} человек;",
                    cs.num_citizen + ce.num_new_citizen - ce.num_dead_citizen);

    std::string new_bushels_str;
    if (cs.num_acre_with_wheat > 0) {
      new_bushels_str = std::format(
          "Мы собрали {} бушелей пшеницы, по {} бушеля с акра;",
          get_num_cultivate_bushels(cs, ce), ce.bushels_wheat_per_acre);
    } else {
      new_bushels_str =
          "Мы не засадили ни одного акра, поэтому ничего не собрали";
    }

    int32_t bushels_wheat = cs.bushels_wheat +
                            get_num_cultivate_bushels(cs, ce) -
                            ce.bushels_wheat_eaten_rats;

    std::string eat_bushels_str;
    if (ce.bushels_wheat_eaten_rats > 0) {
      eat_bushels_str = std::format(
          "Крысы истребили {} бушелей пшеницы, оставив {} бушеля в амбарах;",
          ce.bushels_wheat_eaten_rats, bushels_wheat);
    } else {
      eat_bushels_str =
          std::format("Крысы не потревожили нас, поэтому осталось {} бушеля в амбарах;",
                      bushels_wheat);
    }

    std::string num_acre_str = std::format("Город сейчас занимает {} акров;", cs.num_acre);

    std::string price_acre_str =
        std::format("1 акр земли стоит сейчас {} бушель;", ce.acre_price);

    auto msg = std::vformat(
        std::string_view(tmpl),
        std::make_format_args(rs.num_round, dead_citizen_str, new_citizen_str, plague_str,
                              num_citizen_str, new_bushels_str, eat_bushels_str,
                              num_acre_str, price_acre_str));

    std::println("{}", msg);
}

RulersDecisions get_rulers_decisions(std::istream& in,
                                     RoundState const& round_state) {
  RulersDecisions rd{
      .num_acre_to_buy = 0,
      .num_acre_to_sold = 0,
      .bushels_wheat_to_citizen = 0,
      .num_acre_to_plant = 0,
  };

  CityState const& cs = round_state.city_state;
  CityEvents const& ce = round_state.city_events.value();

  rd.num_acre_to_buy = read_num_acre_to_buy_with_retry(in, cs, ce);
  rd.num_acre_to_sold = read_num_acre_to_sold_with_retry(in, cs, ce);
  rd.bushels_wheat_to_citizen =
      read_bushels_wheat_to_citizen_with_retry(in, cs, ce, rd);
  rd.num_acre_to_plant = read_num_acre_to_plant_with_retry(in, cs, ce, rd);

  return rd;
}

RoundState get_next_round(RoundState const& prev_round) {
  CityState next_city_state{
      .num_acre = 0,
      .num_acre_with_wheat = 0,
      .num_citizen = 0,
      .bushels_wheat = 0,
  };

  next_city_state.num_acre = prev_round.city_state.num_acre +
                             prev_round.rulers_decisions->num_acre_to_buy +
                             prev_round.rulers_decisions->num_acre_to_sold;

  RoundState next_round{.num_round = prev_round.num_round + 1,
                        .city_state = CityState{.num_acre = 1000,
                                                .num_citizen = 100,
                                                .bushels_wheat = 2800},
                        .city_events = std::nullopt,
                        .rulers_decisions = std::nullopt};

  return next_round;
}

template <>
struct std::formatter<CityState> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const CityState& v, std::format_context& ctx) const {
    return std::format_to(ctx.out(),
                          "CityState[num_acre={}, num_acre_with_wheat={}, "
                          "num_citizen={}, bushels_wheat={}]",
                          v.num_acre, v.num_acre_with_wheat, v.num_citizen,
                          v.bushels_wheat);
  }
};

template <>
struct std::formatter<CityEvents> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const CityEvents& rs, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "CityEvents[bushels_wheat_eaten_rats={}, num_dead_citizen={}, "
        "num_new_citizen={}, plague={}, bushels_wheat_per_acre={}, "
        "acre_price={}]",
        rs.bushels_wheat_eaten_rats, rs.num_dead_citizen, rs.num_new_citizen,
        rs.plague, rs.bushels_wheat_per_acre, rs.acre_price);
  }
};

template <>
struct std::formatter<RulersDecisions> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const RulersDecisions& rs, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "RulersDecisions[num_acre_to_buy={}, num_acre_to_sold={}, "
        "bushels_wheat_to_citizen={}, num_acre_to_plant={}]",
        rs.num_acre_to_buy, rs.num_acre_to_sold, rs.bushels_wheat_to_citizen,
        rs.num_acre_to_plant);
  }
};

template <>
struct std::formatter<RoundState> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const RoundState& rs, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "RoundState[num_round={}, city_state={}, "
        "city_events={}, rulers_decisions={}]",
        rs.num_round, rs.city_state,
        rs.city_events ? std::format("{}", *rs.city_events) : "none",
        rs.rulers_decisions ? std::format("{}", *rs.city_events) : "none");
  }
};

void run_game() {
  int seed = 1;
  GameState state = new_game_state();
  while (state.size() < FINAL_ROUND) {
    auto current_round = state.back();

    if (!is_valid_num_round(current_round.num_round)) {
      std::cout << "Номер раунда не находится в диапозоне 1-10" << std::endl;
      break;
    }

    if (round_is_over(current_round) && !is_final_round(current_round)) {
      std::cout << "Debug: Раунд уже сыгран" << std::endl;
      // Отрендерить сообщение
      // Добавить новый раунд в state
      continue;
    }

    current_round.city_events =
        gen_new_city_events(seed, current_round.city_state);

    print_message_to_ruler(current_round);

    std::println("current_round after events: {}", current_round);

    return;

    current_round.rulers_decisions =
        get_rulers_decisions(std::cin, current_round);

    std::println("current_round: {}", current_round);
  }

  // TODO Считаем статистики P и L
}

int main() {
  // UTF-8 позволяет кроссплатформенно выводить русские символы в консоль
  std::locale::global(std::locale("en_US.UTF-8"));

  int n;
  while (true) {
    menu();
    std::cin >> n;
    if (n == 1) {
      run_game();
    } else {
      std::cout << "Debug: Не реализовано" << std::endl;
      return 0;
    }
  }
  return 0;
}
