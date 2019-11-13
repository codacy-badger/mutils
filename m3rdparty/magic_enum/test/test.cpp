// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define MAGIC_ENUM_RANGE_MIN -120
#define MAGIC_ENUM_RANGE_MAX 120
#include <magic_enum.hpp>

#include <array>
#include <string_view>
#include <sstream>

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

enum class Numbers : int { one = 1, two, three, many = 127 };

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

enum number : unsigned long { one = 100, two = 200, three = 300, four = 400 };

namespace magic_enum {
template <>
struct enum_range<number> {
  static constexpr int min = 100;
  static constexpr int max = 300;
};
}

using namespace magic_enum;

static_assert(is_magic_enum_supported, "magic_enum: Unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");

TEST_CASE("enum_cast") {
  SECTION("string") {
    constexpr auto cr = enum_cast<Color>("RED");
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>("GREEN").value() == Color::GREEN);
    REQUIRE(enum_cast<Color>("BLUE").value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>("None").has_value());

    constexpr auto no = enum_cast<Numbers>("one");
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>("two").value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>("three").value() == Numbers::three);
    REQUIRE_FALSE(enum_cast<Numbers>("many").has_value());
    REQUIRE_FALSE(enum_cast<Numbers>("None").has_value());

    constexpr auto dr = enum_cast<Directions>("Right");
    REQUIRE(enum_cast<Directions&>("Up").value() == Directions::Up);
    REQUIRE(enum_cast<Directions>("Down").value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>("Left").value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>("None").has_value());

    constexpr auto nt = enum_cast<number>("three");
    REQUIRE(enum_cast<number>("one").value() == number::one);
    REQUIRE(enum_cast<number>("two").value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(enum_cast<number>("four").has_value());
    REQUIRE_FALSE(enum_cast<number>("None").has_value());
  }

  SECTION("integer") {
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    constexpr auto cr = enum_cast<Color>(-12);
    REQUIRE(cr.value() == Color::RED);
    REQUIRE(enum_cast<Color&>(7).value() == Color::GREEN);
    REQUIRE(enum_cast<Color>((int)cm[2]).value() == Color::BLUE);
    REQUIRE_FALSE(enum_cast<Color>(0).has_value());

    constexpr auto no = enum_cast<Numbers>(1);
    REQUIRE(no.value() == Numbers::one);
    REQUIRE(enum_cast<Numbers>(2).value() == Numbers::two);
    REQUIRE(enum_cast<Numbers>(3).value() == Numbers::three);
    REQUIRE_FALSE(enum_cast<Numbers>(127).has_value());
    REQUIRE_FALSE(enum_cast<Numbers>(0).has_value());

    constexpr auto dr = enum_cast<Directions>(120);
    REQUIRE(enum_cast<Directions&>(85).value() == Directions::Up);
    REQUIRE(enum_cast<Directions>(-42).value() == Directions::Down);
    REQUIRE(dr.value() == Directions::Right);
    REQUIRE(enum_cast<Directions>(-120).value() == Directions::Left);
    REQUIRE_FALSE(enum_cast<Directions>(0).has_value());

    constexpr auto nt = enum_cast<number>(300);
    REQUIRE(enum_cast<number>(100).value() == number::one);
    REQUIRE(enum_cast<number>(200).value() == number::two);
    REQUIRE(nt.value() == number::three);
    REQUIRE_FALSE(enum_cast<number>(400).has_value());
    REQUIRE_FALSE(enum_cast<number>(0).has_value());
  }
}

TEST_CASE("enum_integer") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_integer(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr == -12);
  REQUIRE(enum_integer<Color&>(cg) == 7);
  REQUIRE(enum_integer(cm[2]) == 15);
  REQUIRE(enum_integer(static_cast<Color>(0)) == 0);

  constexpr auto no = enum_integer(Numbers::one);
  REQUIRE(no == 1);
  REQUIRE(enum_integer(Numbers::two) == 2);
  REQUIRE(enum_integer(Numbers::three) == 3);
  REQUIRE(enum_integer(Numbers::many) == 127);
  REQUIRE(enum_integer(static_cast<Numbers>(0)) == 0);

  constexpr auto dr = enum_integer(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_integer<Directions&>(dl) == -120);
  REQUIRE(enum_integer(Directions::Down) == -42);
  REQUIRE(enum_integer(Directions::Up) == 85);
  REQUIRE(dr == 120);
  REQUIRE(enum_integer(static_cast<Directions>(0)) == 0);

  constexpr auto nt = enum_integer(number::three);
  REQUIRE(enum_integer(number::one) == 100);
  REQUIRE(enum_integer(number::two) == 200);
  REQUIRE(nt == 300);
  REQUIRE(enum_integer(number::four) == 400);
  REQUIRE(enum_integer(static_cast<number>(0)) == 0);
}

TEST_CASE("enum_index") {
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  constexpr auto cr = enum_index(Color::RED);
  Color cg = Color::GREEN;
  REQUIRE(cr.value() == 0);
  REQUIRE(enum_index<Color&>(cg).value() == 1);
  REQUIRE(enum_index(cm[2]).value() == 2);
  REQUIRE_FALSE(enum_index(static_cast<Color>(0)).has_value());

  constexpr auto no = enum_index(Numbers::one);
  REQUIRE(no.value() == 0);
  REQUIRE(enum_index(Numbers::two).value() == 1);
  REQUIRE(enum_index(Numbers::three).value() == 2);
  REQUIRE_FALSE(enum_index(Numbers::many).has_value());
  REQUIRE_FALSE(enum_index(static_cast<Numbers>(0)).has_value());

  constexpr auto dr = enum_index(Directions::Right);
  Directions dl = Directions::Left;
  REQUIRE(enum_index<Directions&>(dl).value() == 0);
  REQUIRE(enum_index(Directions::Down).value() == 1);
  REQUIRE(enum_index(Directions::Up).value() == 2);
  REQUIRE(dr.value() == 3);
  REQUIRE_FALSE(enum_index(static_cast<Directions>(0)).has_value());

  constexpr auto nt = enum_index(number::three);
  REQUIRE(enum_index(number::one).value() == 0);
  REQUIRE(enum_index(number::two).value() == 1);
  REQUIRE(nt.value() == 2);
  REQUIRE_FALSE(enum_index(number::four).has_value());
  REQUIRE_FALSE(enum_index(static_cast<number>(0)).has_value());
}

TEST_CASE("enum_value") {
  constexpr auto cr = enum_value<Color>(0);
  REQUIRE(cr == Color::RED);
  REQUIRE(enum_value<Color&>(1) == Color::GREEN);
  REQUIRE(enum_value<Color>(2) == Color::BLUE);

  constexpr auto no = enum_value<Numbers>(0);
  REQUIRE(no == Numbers::one);
  REQUIRE(enum_value<Numbers>(1) == Numbers::two);
  REQUIRE(enum_value<Numbers>(2) == Numbers::three);

  constexpr auto dr = enum_value<Directions>(3);
  REQUIRE(enum_value<Directions&>(0) == Directions::Left);
  REQUIRE(enum_value<Directions>(1) == Directions::Down);
  REQUIRE(enum_value<Directions>(2) == Directions::Up);
  REQUIRE(dr == Directions::Right);

  constexpr auto nt = enum_value<number>(2);
  REQUIRE(enum_value<number>(0) == number::one);
  REQUIRE(enum_value<number>(1) == number::two);
  REQUIRE(nt == number::three);
}

TEST_CASE("enum_values") {
  constexpr auto s1 = enum_values<Color&>();
  REQUIRE(s1 == std::array<Color, 3>{{Color::RED, Color::GREEN, Color::BLUE}});

  auto s2 = enum_values<Numbers>();
  REQUIRE(s2 == std::array<Numbers, 3>{{Numbers::one, Numbers::two, Numbers::three}});

  constexpr auto s3 = enum_values<Directions&>();
  REQUIRE(s3 == std::array<Directions, 4>{{Directions::Left, Directions::Down, Directions::Up, Directions::Right}});

  auto s4 = enum_values<number>();
  REQUIRE(s4 == std::array<number, 3>{{number::one, number::two, number::three}});
}

TEST_CASE("enum_count") {
  constexpr auto s1 = enum_count<Color&>();
  REQUIRE(s1 == 3);

  auto s2 = enum_count<Numbers>();
  REQUIRE(s2 == 3);

  constexpr auto s3 = enum_count<Directions&>();
  REQUIRE(s3 == 4);

  auto s4 = enum_count<number>();
  REQUIRE(s4 == 3);
}

TEST_CASE("enum_name") {
  SECTION("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    Color cb = Color::BLUE;
    REQUIRE(cr_name == "RED");
    REQUIRE(enum_name<Color&>(cb) == "BLUE");
    REQUIRE(enum_name(cm[1]) == "GREEN");
    REQUIRE(enum_name(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name(no);
    REQUIRE(no_name == "one");
    REQUIRE(enum_name(Numbers::two) == "two");
    REQUIRE(enum_name(Numbers::three) == "three");
    REQUIRE(enum_name(Numbers::many).empty());
    REQUIRE(enum_name(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name(dr);
    Directions du = Directions::Up;
    REQUIRE(enum_name<Directions&>(du) == "Up");
    REQUIRE(enum_name(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name(Directions::Left) == "Left");
    REQUIRE(enum_name(static_cast<Directions>(0)).empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name(nt);
    REQUIRE(enum_name(number::one) == "one");
    REQUIRE(enum_name(number::two) == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(enum_name(number::four).empty());
    REQUIRE(enum_name(static_cast<number>(0)).empty());
  }

  SECTION("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = enum_name<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(enum_name<Color::BLUE>() == "BLUE");
    REQUIRE(enum_name<cm[1]>() == "GREEN");
    REQUIRE(enum_name<static_cast<Color>(0)>().empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = enum_name<no>();
    REQUIRE(no_name == "one");
    REQUIRE(enum_name<Numbers::two>() == "two");
    REQUIRE(enum_name<Numbers::three>() == "three");
    REQUIRE(enum_name<Numbers::many>() == "many");
    REQUIRE(enum_name<static_cast<Numbers>(0)>().empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = enum_name<dr>();
    REQUIRE(enum_name<Directions::Up>() == "Up");
    REQUIRE(enum_name<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(enum_name<Directions::Left>() == "Left");
    REQUIRE(enum_name<static_cast<Directions>(0)>().empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = enum_name<nt>();
    REQUIRE(enum_name<number::one>() == "one");
    REQUIRE(enum_name<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(enum_name<number::four>() == "four");
    REQUIRE(enum_name<static_cast<number>(0)>().empty());
  }
}

TEST_CASE("enum_names") {
  constexpr auto s1 = enum_names<Color&>();
  REQUIRE(s1 == std::array<std::string_view, 3>{{"RED", "GREEN", "BLUE"}});

  auto s2 = enum_names<Numbers>();
  REQUIRE(s2 == std::array<std::string_view, 3>{{"one", "two", "three"}});

  constexpr auto s3 = enum_names<Directions&>();
  REQUIRE(s3 == std::array<std::string_view, 4>{{"Left", "Down", "Up", "Right"}});

  auto s4 = enum_names<number>();
  REQUIRE(s4 == std::array<std::string_view, 3>{{"one", "two", "three"}});
}

TEST_CASE("enum_entries") {
  constexpr auto s1 = enum_entries<Color&>();
  REQUIRE(s1 == std::array<std::pair<Color, std::string_view>, 3>{{{Color::RED, "RED"}, {Color::GREEN, "GREEN"}, {Color::BLUE, "BLUE"}}});

  auto s2 = enum_entries<Numbers>();
  REQUIRE(s2 == std::array<std::pair<Numbers, std::string_view>, 3>{{{Numbers::one, "one"}, {Numbers::two, "two"}, {Numbers::three, "three"}}});

  constexpr auto s3 = enum_entries<Directions&>();
  REQUIRE(s3 == std::array<std::pair<Directions, std::string_view>, 4>{{{Directions::Left, "Left"}, {Directions::Down, "Down"}, {Directions::Up, "Up"}, {Directions::Right, "Right"}}});

  auto s4 = enum_entries<number>();
  REQUIRE(s4 == std::array<std::pair<number, std::string_view>, 3>{{{number::one, "one"}, {number::two, "two"}, {number::three, "three"}}});
}

TEST_CASE("ostream_operators") {
  auto test_ostream = [](auto e, std::string_view name) {
    using namespace magic_enum::ostream_operators;
    std::stringstream ss;
    ss << e;
    REQUIRE(ss.str() == name);
  };

  test_ostream(std::make_optional(Color::RED), "RED");
  test_ostream(Color::GREEN, "GREEN");
  test_ostream(Color::BLUE, "BLUE");
  test_ostream(static_cast<Color>(0), "0");
  test_ostream(std::make_optional(static_cast<Color>(0)), "0");

  test_ostream(std::make_optional(Numbers::one), "one");
  test_ostream(Numbers::two, "two");
  test_ostream(Numbers::three, "three");
  test_ostream(Numbers::many, "127");
  test_ostream(static_cast<Numbers>(0), "0");
  test_ostream(std::make_optional(static_cast<Numbers>(0)), "0");

  test_ostream(std::make_optional(Directions::Up), "Up");
  test_ostream(Directions::Down, "Down");
  test_ostream(Directions::Right, "Right");
  test_ostream(Directions::Left, "Left");
  test_ostream(static_cast<Directions>(0), "0");
  test_ostream(std::make_optional(static_cast<Directions>(0)), "0");

  test_ostream(std::make_optional(number::one), "one");
  test_ostream(number::two, "two");
  test_ostream(number::three, "three");
  test_ostream(number::four, "400");
  test_ostream(static_cast<number>(0), "0");
  test_ostream(std::make_optional(static_cast<number>(0)), "0");
}

TEST_CASE("bitwise_operators") {
  using namespace magic_enum::bitwise_operators;

  SECTION("operator^") {
    REQUIRE(enum_integer(~Color::RED) == ~enum_integer(Color::RED));
    REQUIRE(enum_integer(~Numbers::one) == ~enum_integer(Numbers::one));
    REQUIRE(enum_integer(~Directions::Up) == ~enum_integer(Directions::Up));
    REQUIRE(enum_integer(~number::one) == ~enum_integer(number::one));
  }

  SECTION("operator|") {
    REQUIRE(enum_integer(Color::RED | Color::BLUE) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one | Numbers::two) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up | Directions::Down) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one | number::two) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SECTION("operator&") {
    REQUIRE(enum_integer(Color::RED & Color::BLUE) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one & Numbers::two) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up & Directions::Down) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one & number::two) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SECTION("operator^") {
    REQUIRE(enum_integer(Color::RED ^ Color::BLUE) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));
    REQUIRE(enum_integer(Numbers::one ^ Numbers::two) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));
    REQUIRE(enum_integer(Directions::Up ^ Directions::Down) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));
    REQUIRE(enum_integer(number::one ^ number::two) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }

  SECTION("operator|=") {
    Color x1 = Color::RED;
    x1 |= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) | enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 |= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) | enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 |= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) | enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 |= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) | enum_integer(number::two)));
  }

  SECTION("operator&=") {
    Color x1 = Color::RED;
    x1 &= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) & enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 &= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) & enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 &= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) & enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 &= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) & enum_integer(number::two)));
  }

  SECTION("operator^=") {
    Color x1 = Color::RED;
    x1 ^= Color::BLUE;
    REQUIRE(enum_integer(x1) == (enum_integer(Color::RED) ^ enum_integer(Color::BLUE)));

    Numbers x2 = Numbers::one;
    x2 ^= Numbers::two;
    REQUIRE(enum_integer(x2) == (enum_integer(Numbers::one) ^ enum_integer(Numbers::two)));

    Directions x3 = Directions::Up;
    x3 ^= Directions::Down;
    REQUIRE(enum_integer(x3) == (enum_integer(Directions::Up) ^ enum_integer(Directions::Down)));

    number x4 = number::one;
    x4 ^= number::two;
    REQUIRE(enum_integer(x4) == (enum_integer(number::one) ^ enum_integer(number::two)));
  }
}

TEST_CASE("type_traits") {
  REQUIRE_FALSE(is_unscoped_enum_v<Color>);
  REQUIRE_FALSE(is_unscoped_enum_v<Numbers>);
  REQUIRE(is_unscoped_enum_v<Directions>);
  REQUIRE(is_unscoped_enum_v<number>);

  REQUIRE(is_scoped_enum_v<Color>);
  REQUIRE(is_scoped_enum_v<Numbers>);
  REQUIRE_FALSE(is_scoped_enum_v<Directions>);
  REQUIRE_FALSE(is_scoped_enum_v<number>);
}

TEST_CASE("enum_traits") {
  SECTION("type_name") {
    REQUIRE(enum_traits<Color&>::type_name == "Color");
    REQUIRE(enum_traits<Numbers>::type_name == "Numbers");
    REQUIRE(enum_traits<Directions&>::type_name == "Directions");
    REQUIRE(enum_traits<number>::type_name == "number");
  }
}

TEST_CASE("extrema") {
  enum class BadColor : std::uint64_t {
    RED,
    GREEN,
    YELLOW,
    // The value NONE is ignored (out of range).
    // However, it affects the value of min_v. When reflected_min_v was incorrect,
    // the presence of NONE caused miv_v to be equal to -1, which was then cast to unsigned,
    // leading to a value of 18446744073709551615 (numeric_limit_max of uint64_t).
    NONE = std::numeric_limits<std::uint64_t>::max()
  };

  SECTION("min") {
    REQUIRE(magic_enum::enum_range<BadColor>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<BadColor> == 0);
    REQUIRE(magic_enum::detail::min_v<BadColor> == 0);

    REQUIRE(magic_enum::enum_range<Color>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Color> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Color> == -12);

    REQUIRE(magic_enum::enum_range<Numbers>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Numbers> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Numbers> == 1);

    REQUIRE(magic_enum::enum_range<Directions>::min == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::reflected_min_v<Directions> == MAGIC_ENUM_RANGE_MIN);
    REQUIRE(magic_enum::detail::min_v<Directions> == -120);

#if defined(__clang__) && __clang_major__ == 5
    // clang-5 linker fail: undefined reference to `magic_enum::enum_range<number>::min'.
#else
    REQUIRE(magic_enum::enum_range<number>::min == 100);
#endif
    REQUIRE(magic_enum::detail::reflected_min_v<number> == 100);
    REQUIRE(magic_enum::detail::min_v<number> == 100);
  }

  SECTION("max") {
    REQUIRE(magic_enum::enum_range<BadColor>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<BadColor> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<BadColor> == 2);

    REQUIRE(magic_enum::enum_range<Color>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Color> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Color> == 15);

    REQUIRE(magic_enum::enum_range<Numbers>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Numbers> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Numbers> == 3);

    REQUIRE(magic_enum::enum_range<Directions>::max == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::reflected_max_v<Directions> == MAGIC_ENUM_RANGE_MAX);
    REQUIRE(magic_enum::detail::max_v<Directions> == 120);

#if defined(__clang__) && __clang_major__ == 5
    // clang-5 linker fail: undefined reference to `magic_enum::enum_range<number>::max'.
#else
    REQUIRE(magic_enum::enum_range<number>::max == 300);
#endif
    REQUIRE(magic_enum::detail::reflected_max_v<number> == 300);
    REQUIRE(magic_enum::detail::max_v<number> == 300);
  }
}

TEST_CASE("mixed_sign_less") {
  using magic_enum::detail::mixed_sign_less;

  constexpr std::uint64_t uint64_t_min = std::numeric_limits<std::uint64_t>::min();
  constexpr std::uint32_t uint32_t_min = std::numeric_limits<std::uint32_t>::min();
  constexpr std::uint32_t uint32_t_max = std::numeric_limits<std::uint32_t>::max();
  constexpr std::uint64_t uint64_t_max = std::numeric_limits<std::uint64_t>::max();

  constexpr std::int64_t int64_t_min = std::numeric_limits<std::int64_t>::min();
  constexpr std::int32_t int32_t_min = std::numeric_limits<std::int32_t>::min();
  constexpr std::int32_t int32_t_max = std::numeric_limits<std::int32_t>::max();
  constexpr std::int64_t int64_t_max = std::numeric_limits<std::int64_t>::max();

  // Also testing with offset to avoid corner cases.
  // Two variables to avoid hidden casts:
  constexpr std::int64_t offset_int64_t = 17;
  constexpr std::int32_t offset_int32_t = 17;

  SECTION("same signedness") {
    REQUIRE(mixed_sign_less(-5, -3));
    REQUIRE(mixed_sign_less(27U, 49U));
  }

  SECTION("same signedness, different width") {
    REQUIRE(mixed_sign_less(uint32_t_max, uint64_t_max));
    REQUIRE_FALSE(mixed_sign_less(uint64_t_max, uint32_t_max));
    REQUIRE(mixed_sign_less(int64_t_min, int32_t_min));
    REQUIRE_FALSE(mixed_sign_less(int32_t_min, int64_t_min));
    REQUIRE(mixed_sign_less(int64_t_min + offset_int64_t, int32_t_min + offset_int32_t));
    REQUIRE_FALSE(mixed_sign_less(int32_t_min + offset_int32_t, int64_t_min + offset_int64_t));
  }

  SECTION("left signed, right unsigned") {
    REQUIRE(mixed_sign_less(-5, 3U));
    REQUIRE(mixed_sign_less(3, 5U));
  }

  SECTION("left signed, right unsigned, different width") {
    REQUIRE(mixed_sign_less(int32_t_max, uint64_t_max));
    REQUIRE_FALSE(mixed_sign_less(int64_t_max, uint32_t_max));
    REQUIRE(mixed_sign_less(int32_t_min, uint64_t_min));
    REQUIRE(mixed_sign_less(int64_t_min, uint32_t_min));
    REQUIRE(mixed_sign_less(int32_t_max - offset_int32_t, uint64_t_max));
    REQUIRE_FALSE(mixed_sign_less(int64_t_max - offset_int64_t, uint32_t_max));
    REQUIRE(mixed_sign_less(int32_t_min + offset_int32_t, uint64_t_min));
    REQUIRE(mixed_sign_less(int64_t_min + offset_int64_t, uint32_t_min));
  }

  SECTION("left unsigned, right signed") {
    REQUIRE_FALSE(mixed_sign_less(3U, -5));
    REQUIRE(mixed_sign_less(3U, 5));
  }

  SECTION("left unsigned, right signed, different width") {
    REQUIRE(mixed_sign_less(uint32_t_max, int64_t_max));
    REQUIRE_FALSE(mixed_sign_less(uint64_t_max, int32_t_max));
    REQUIRE_FALSE(mixed_sign_less(uint32_t_min, int64_t_min));
    REQUIRE_FALSE(mixed_sign_less(uint64_t_min, int32_t_min));
    REQUIRE(mixed_sign_less(uint32_t_max, int64_t_max - offset_int32_t));
    REQUIRE_FALSE(mixed_sign_less(uint64_t_max, int32_t_max - offset_int64_t));
    REQUIRE_FALSE(mixed_sign_less(uint32_t_min, int64_t_min + offset_int32_t));
    REQUIRE_FALSE(mixed_sign_less(uint64_t_min, int32_t_min + offset_int64_t));
  }
}
