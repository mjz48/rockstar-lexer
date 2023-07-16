#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

#include "lexer.h"
#include "rockstar.h"

Lexer::AliasMap Lexer::keyword_aliases = {
  //std::make_pair(),
  std::make_pair("nothing", "null"),
  std::make_pair("nowhere", "null"),
  std::make_pair("nobody", "null"),
  std::make_pair("gone", "null"),
  std::make_pair("right", "true"),
  std::make_pair("yes", "true"),
  std::make_pair("ok", "true"),
  std::make_pair("wrong", "false"),
  std::make_pair("no", "false"),
  std::make_pair("lies", "false"),
  std::make_pair("empty", ""), // this might not belong here?
  std::make_pair("silent", ""), // this might not belong here?
  std::make_pair("silence", ""), // this might not belong here?
  std::make_pair("push", "rock"),
  std::make_pair("pop", "roll"),
  std::make_pair("split", "cut"),
  std::make_pair("shatter", "cut"),
  std::make_pair("unite", "join"),
  std::make_pair("burn", "cast"),
  // leave out [with -> +] and [without => -] aliases because those
  // overlap with context sensitive keywords
  std::make_pair("are", "is"),
  std::make_pair("was", "is"),
  std::make_pair("were", "is"),
  std::make_pair("aint", "isnt"),
  std::make_pair("arent", "isnt"),
  std::make_pair("wasnt", "isnt"),
  std::make_pair("werent", "isnt"),
};

Lexer::KeywordMap Lexer::keywords = {
  std::make_pair("a", TokenType::A),
  std::make_pair("an", TokenType::AN),
  std::make_pair("the", TokenType::THE),
  std::make_pair("my", TokenType::MY),
  std::make_pair("your", TokenType::YOUR),
  std::make_pair("our", TokenType::OUR),
  std::make_pair("it", TokenType::IT),
  std::make_pair("he", TokenType::HE),
  std::make_pair("she", TokenType::SHE),
  std::make_pair("him", TokenType::HIM),
  std::make_pair("her", TokenType::HER),
  std::make_pair("they", TokenType::THEY),
  std::make_pair("them", TokenType::THEM),
  std::make_pair("ze", TokenType::ZE),
  std::make_pair("hir", TokenType::HIR),
  std::make_pair("zie", TokenType::ZIE),
  std::make_pair("zir", TokenType::ZIR),
  std::make_pair("xe", TokenType::XE),
  std::make_pair("xem", TokenType::XEM),
  std::make_pair("ve", TokenType::VE),
  std::make_pair("ver", TokenType::VER),
  std::make_pair("put", TokenType::PUT),
  std::make_pair("into", TokenType::INTO),
  std::make_pair("of", TokenType::OF),
  std::make_pair("in", TokenType::IN),
  std::make_pair("is", TokenType::IS),
  std::make_pair("isnt", TokenType::ISNT),
  std::make_pair("let", TokenType::LET),
  std::make_pair("be", TokenType::BE),
  std::make_pair("at", TokenType::AT),
  std::make_pair("rock", TokenType::ROCK),
  std::make_pair("roll", TokenType::ROLL),
  std::make_pair("with", TokenType::WITH),
  std::make_pair("without", TokenType::WITHOUT),
  std::make_pair("like", TokenType::LIKE),
  std::make_pair("cut", TokenType::CUT),
  std::make_pair("join", TokenType::JOIN),
  std::make_pair("cast", TokenType::CAST),
  std::make_pair("build", TokenType::BUILD),
  std::make_pair("knock", TokenType::KNOCK),
  std::make_pair("turn", TokenType::TURN),
  std::make_pair("up", TokenType::UP),
  std::make_pair("down", TokenType::DOWN),
  std::make_pair("around", TokenType::AROUND),
  std::make_pair("say", TokenType::SAY),
  std::make_pair("listen", TokenType::LISTEN),
  std::make_pair("and", TokenType::AND),
  std::make_pair("or", TokenType::OR),
  std::make_pair("nor", TokenType::NOR),
  std::make_pair("not", TokenType::NOT),
  std::make_pair("as", TokenType::AS),
  std::make_pair("high", TokenType::HIGH),
  std::make_pair("higher", TokenType::HIGHER),
  std::make_pair("low", TokenType::LOW),
  std::make_pair("lower", TokenType::LOWER),
  std::make_pair("than", TokenType::THAN),
  std::make_pair("if", TokenType::IF),
  std::make_pair("else", TokenType::ELSE),
  std::make_pair("while", TokenType::WHILE),
  std::make_pair("break", TokenType::BREAK),
  std::make_pair("continue", TokenType::CONTINUE),
  std::make_pair("takes", TokenType::TAKES),
  std::make_pair("taking", TokenType::TAKING),
  std::make_pair("return", TokenType::RETURN),
  std::make_pair("mysterious", TokenType::MYSTERIOUS),
  std::make_pair("null", TokenType::NULL_TOKEN),
  std::make_pair("true", TokenType::TRUE),
  std::make_pair("false", TokenType::FALSE),
  std::make_pair("empty", TokenType::EMPTY),
  std::make_pair("plus", TokenType::PLUS),
  std::make_pair("minus", TokenType::MINUS),
};

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

bool Lexer::is_keyword(std::string keyword) {
  try {
    this->get_keyword(keyword);
    return true;
  } catch (const std::out_of_range& e) {
    return false;
  }
}

TokenType Lexer::get_keyword(std::string keyword) {
  std::string keyword_lower;
  for (char c : keyword) {
    keyword_lower += tolower(c);
  }

  // do alias substitution
  auto alias_it = this->keyword_aliases.find(keyword_lower);
  if (alias_it != this->keyword_aliases.end()) {
    keyword_lower = alias_it->second;
  }

  return Lexer::keywords.at(keyword_lower);
}

void Lexer::scan_token() {
  char c = this->advance();
  Location curr_loc(this->current, 1);

  switch(c) {
    case ' ':
    case '\t':
    case '\r':
      // ignore whitespace
      break;
    case '\n':
      this->parse_newline();
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
        std::stringstream msg;
        msg << "Unexpected character '" << c << "'.";

        Rockstar::error(curr_loc, msg.str());
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

void Lexer::parse_newline() {
  if (this->peek() != '\n' && this->peek(1) != '\r' && this->peek(2) != '\n') {
    return;
  }

  // capture newlines
  this->advance();
  this->advance();
  this->advance();

  this->add_token(TokenType::EMPTY_LINE);
}

void Lexer::parse_comment() {
  while (this->peek() != ')' && !this->is_at_end()) {
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
  while (this->peek() != '"' && !this->is_at_end()) {
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
  while (this->is_alpha(this->peek())) {
    this->advance();
  }

  std::string text = this->source.substr(this->start, this->current - this->start);

  if (!this->is_keyword(text)) {
    std::string text_lower;
    for (char c : text) {
      text_lower += tolower(c);
    }

    // did not find any keyword match, log this as an identifier
    this->add_token(TokenType::IDENTIFIER, text_lower);
  } else {
    TokenType token = this->get_keyword(text);

    // handle common variables
    std::vector<TokenType> common_prefixes = {
      TokenType::A,
      TokenType::AN,
      TokenType::THE,
      TokenType::MY,
      TokenType::YOUR,
      TokenType::OUR
    };

    auto search = std::find(common_prefixes.begin(), common_prefixes.end(), token);
    if (search != common_prefixes.end()) {
      // find start of next identifier (should be only whitespace)
      while (!this->is_alpha(this->peek())) {
        this->advance();
      }

      int next_start = this->current;

      // consume next token
      while (this->is_alpha(this->peek())) {
        this->advance();
      }

      std::string next_identifier = this->source.substr(next_start, this->current - next_start);
      if (this->is_keyword(next_identifier)) {
        Location loc(next_start, this->current - next_start);

        std::stringstream msg;
        msg << "Expected identifier after article '" << token;
        msg << "' for common variable. Found '" << next_identifier;
        msg << "' instead.";

        Rockstar::error(loc, msg.str());
      } else {
        // create common variable token
        this->add_token(TokenType::IDENTIFIER, text + " " + next_identifier);
      }
    } else {
      this->add_token(token);
    }
  }
}
