#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <stdint.h>

class Location {
  public:
    uint32_t offset;
    uint32_t length;

    Location(uint32_t offset, uint32_t length)
    : offset(offset), length(length) {}
};

inline std::ostream& operator<<(std::ostream& str, const Location& l) {
  str << "[Location (" << l.offset << ", " << l.length << ")]";
  return str;
}

#endif
