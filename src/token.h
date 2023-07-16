#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <iostream>
#include <stdint.h>
#include <string>
#include <variant>

#include "util.h"

#include "magic_enum.hpp"

// used for storing data associated with literal tokens
using TokenData = std::variant<std::monostate, std::string, double>;

inline std::ostream& operator<<(std::ostream& str, const TokenData& tt) {
  std::visit([&str](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;

    if constexpr (std::is_same_v<T, std::string>) {
      str << arg;
    } else if constexpr (std::is_arithmetic_v<T>) {
      str << std::to_string(arg);
    }
  }, tt);

  return str;
}

enum class TokenType {
  // single character tokens
  DOT, PLUS, MINUS, STAR, SLASH, COMMA, EMPTY_LINE,

  // literals
  COMMENT, IDENTIFIER, STRING, NUMBER,

  // keywords
  A, AN, THE, MY, YOUR, OUR,
  IT, HE, SHE, HIM, HER, THEY, THEM, ZE, HIR, ZIE, ZIR, XE, XEM, VE, VER,
  PUT, INTO, OF, IN, IS, ISNT,
  LET, BE, AT,
  ROCK, ROLL, WITH, WITHOUT, LIKE,
  CUT, JOIN,
  CAST,
  BUILD, KNOCK, TURN, UP, DOWN, AROUND,
  SAY, LISTEN,
  AND, OR, NOR, NOT,
  AS, HIGH, HIGHER, LOW, LOWER, THAN,
  IF, ELSE, WHILE, BREAK, CONTINUE,
  TAKES, TAKING, RETURN, BACK,

  // constants
  MYSTERIOUS, NULL_TOKEN, TRUE, FALSE, EMPTY,

  // special
  END_OF_FILE,
};

inline std::ostream& operator<<(std::ostream& str, const TokenType& tt) {
  str << magic_enum::enum_name(tt);
  return str;
}

class Token {
  public:
    Token(TokenType type, std::string lexeme, TokenData data, Location loc)
    : type(type)
    , lexeme(lexeme)
    , data(data)
    , location(loc)
    {}

  private:
    TokenType type;
    std::string lexeme;
    TokenData data;
    Location location;

    friend std::ostream& operator<<(std::ostream& str, const Token& t);
};

inline std::ostream& operator<<(std::ostream& str, const Token& t) {
  str << "[Token { type: " << t.type << ", lexeme: '" << t.lexeme << "', data: '" << t.data << "', loc: " << t.location << "}]";
  return str;
}

#endif
