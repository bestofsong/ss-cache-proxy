//
// Created by wansong on 12/03/2018.
//

#include <iostream>

enum class result {
  great = 0,
  good = 1,
  ok = good << 1,
  works = good << 2,
  useless = good << 3,
  suck = good << 4,
};

int main() {
  std::cout << static_cast<int>(result::great) << std::endl;
  std::cout << static_cast<int>(result::good) << std::endl;
  std::cout << static_cast<int>(result::ok) << std::endl;
  std::cout << static_cast<int>(result::works) << std::endl;
  std::cout << static_cast<int>(result::useless) << std::endl;
  std::cout << static_cast<int>(result::suck) << std::endl;
  return 0;
}