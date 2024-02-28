// File: jdi_error.cpp
// ----
// Easy error handling.

#include "jdi.hpp"

namespace jdi {
  
  const char* noWhere = "";
  
  Error::Error(const char* where) throw() :
    _what(SDL_GetError()),
    _where(where == 0 ? noWhere : where)
  {}
  

  Error::~Error() throw() {}

  const char* Error::what() const throw() {
    return(_what);
  }

  const char* Error::where() const throw() {
    return(_where);
  }
  
}  // end namespace jdi

