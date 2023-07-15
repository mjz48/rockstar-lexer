#ifndef __ROCKSTAR_H__
#define __ROCKSTAR_H__

#include <iostream>

#include "lexer.h"
#include "util.h"

class Rockstar {
  public:
    static void error(Location loc, std::string msg) {
      report(loc, "", msg);
    }

    static void shell() {
      std::string cmd;

      while (true) {
        std::cout << "#> ";
        std::getline(std::cin, cmd);

        Lexer lex(cmd);
        auto tokens = lex.scan_tokens();

        std::cout << "Parsed input:" << std::endl;
        for (auto token : tokens) {
          std::cout << token << std::endl;
        }
      }
    }
    
  private:
    inline static bool hadError = false;
    static void report(Location loc, std::string where, std::string msg) {
      std::cerr << loc << " Error" << where << ": " << msg << std::endl;
      hadError = true;
    }
};

#endif
