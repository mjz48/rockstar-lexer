#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "token.h"
#include "util.h"

class Lexer {
  public:
    Lexer(std::string source);

    bool is_at_end();
    const std::vector<Token>& scan_tokens();

  private:
    char advance();
    char peek(int lookahead = 0);

    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_alphanumeric(char c);
    bool is_keyword(std::string keyword);

    TokenType get_keyword(std::string keyword);

    void scan_token();
    void add_token(TokenType type, TokenData data = std::monostate{});

    void parse_newline();
    void parse_comment();
    void parse_string();
    void parse_number();
    void parse_identifier();

  private:
    using KeywordMap = std::map<std::string, TokenType>;
    static KeywordMap keywords;

    using AliasMap = std::map<std::string, std::string>;
    static AliasMap keyword_aliases;

    int start;
    int current;
    std::string source;
    std::vector<Token> tokens;
};

#endif
