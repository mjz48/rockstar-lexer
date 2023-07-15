#include "lexer.h"

#include "rockstar.h"

Lexer::Lexer(std::string source)
: start(0)
, current(0)
, source(source)
, tokens()
{
}

bool Lexer::is_at_end() {
  return static_cast<long unsigned int>(current) >= source.length();
}

const std::vector<Token>& Lexer::scan_tokens() {
  while (!this->is_at_end()) {
    // we're at the beginning of a new lexeme
    this->start = this->current;
    scan_token();
  }

  this->tokens.push_back(
    Token(
      TokenType::END_OF_FILE,
      "",
      std::monostate{},
      Location(this->source.length(), 0)
  ));
  return this->tokens;
}

char Lexer::advance() {
  return this->source.at(current++);
}

char Lexer::peek(int lookahead) {
  if (this->is_at_end()) {
    return '\0';
  }

  return this->source.at(this->current + lookahead);
}

bool Lexer::is_digit(char c) {
  return c >= '0' && c <= '9';
}

bool Lexer::is_alpha(char c) {
  return (c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || c == '_';
}

bool Lexer::is_alphanumeric(char c) {
  return this->is_digit(c) || this->is_alpha(c);
}

void Lexer::scan_token() {
  char c = this->advance();
  Location curr_loc(this->current, 1);

  switch(c) {
    case ' ':
    case '\r':
    case '\t':
    case '\n':
      // ignore whitespace
      break;
    case '.': this->add_token(TokenType::DOT); break;
    case '+': this->add_token(TokenType::PLUS); break;
    case '-': this->add_token(TokenType::MINUS); break;
    case '*': this->add_token(TokenType::STAR); break;
    case ',': this->add_token(TokenType::COMMA); break;
    case '(': this->parse_comment(); break;
    case '"': this->parse_string(); break;
    default:
      if (this->is_digit(c)) {
        this->parse_number();
      } else if (this->is_alpha(c)) {
        this->parse_identifier();
      } else {
        Rockstar::error(curr_loc, "Unexpected character.");
      }
      break;
  }
}

void Lexer::add_token(TokenType type, TokenData data) {
  std::string token_text = this->source.substr(this->start, this->current - this->start);

  this->tokens.push_back(
      Token(type, token_text, data, Location(this->start, this->current - this->start))
  );
}

void Lexer::parse_comment() {
  while (peek() != ')' && !this->is_at_end()) {
    this->advance();
  }

  // capture the closing parenthesis
  if (this->is_at_end()) {
    Location loc(this->start, this->current - this->start);
    Rockstar::error(loc, "Unterminated comment.");
  } else {
    // capture the closing paren
    this->advance();
  }

  this->add_token(
    TokenType::COMMENT,
    this->source.substr(this->start + 1, this->current - this->start - 2)
  );
}

void Lexer::parse_string() {
  while (peek() != '"' && !this->is_at_end()) {
    this->advance();
  }

  if (this->is_at_end()) {
    Location loc(this->start, this->current - this->start);
    Rockstar::error(loc, "Unterminated string literal.");
  } else {
    // capture the closing double quote
    this->advance();
  }

  this->add_token(
      TokenType::STRING,
      this->source.substr(this->start + 1, this->current - this->start - 2)
  );
}

void Lexer::parse_number() {
  while(this->is_digit(this->peek())) {
    this->advance();
  }

  // second peek is because we don't want to consume the '.' unless
  // we're sure it's part of this number.
  if (this->peek() == '.' && this->is_digit(this->peek(1))) {
    this->advance(); // consume the '.'
  }

  while(this->is_digit(this->peek())) {
    this->advance();
  }

  std::string number_data = this->source.substr(this->start, this->current - this->start);

  // TODO: handle exception flows from std::stod
  this->add_token(
      TokenType::NUMBER,
      std::stod(number_data)
  );
}

void Lexer::parse_identifier() {
  // rockstar identifiers cannot have numbers, spaces, or underscores in them
  while(this->is_alpha(this->peek())) {
    this->advance();
  }

  std::string token_data = this->source.substr(this->start, this->current - this->start);
  this->add_token(TokenType::IDENTIFIER, token_data);
}
