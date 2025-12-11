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

#include "city_state.h"
#include "city_events.h"
#include "rullers_decisions.h"
#include "round_state.h"

void menu() {
  std::cout << "Добро пожаловать в игру Хаммурапи: Правитель Египта." << std::endl;
  std::cout << "Начать игру:" << std::endl;
  std::cout << "1. Новая игра" << std::endl;
  std::cout << "2. Загрузить игру" << std::endl;
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
        "Что пожелаешь, повелитель?";

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

    std::cout << msg << std::endl;
}

enum class GameEndEnum {
    Bad,
    Normal,
    Good,
    Great
};

GameEndEnum is_win(double P, double L) {
  if (P > 33.0 && L < 7.0) return GameEndEnum::Bad;
  if (P > 10.0 && L < 9.0) return GameEndEnum::Normal;
  if (P > 3.0 && L < 10.0) return GameEndEnum::Normal;
  return GameEndEnum::Great;
}

void run_game() {
  int seed = 1;
  GameState state = new_game_state();
  while (state.size() < FINAL_ROUND) {
    auto& current_round = state.back();

    if (!is_valid_round(current_round)) {
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

    current_round.rulers_decisions =
        get_rulers_decisions(std::cin, current_round.city_state, *current_round.city_events);

    state.push_back(get_next_round(current_round));
  }

  double P = calc_mean_dead_persons(state);
  double L = calc_num_acres_per_citizen(state);
  
  std::cout << "Is this the end..." << std::endl;
  std::cout << static_cast<int>(is_win(P, L)) << std::endl;
}

int main() {
  // UTF-8 позволяет кроссплатформенно выводить русские символы в консоль
  std::locale utf8("ru_RU.UTF-8");
  std::locale::global(utf8);

  int n;
  menu();
  std::cin >> n;
  if (n == 1) {
    run_game();
  } else {
    std::cout << "Debug: Не реализовано" << std::endl;
  }
  return 0;
}
