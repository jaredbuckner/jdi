// File: jdi_error.cpp
// ----
// Easy error handling.

#include "jdi.hpp"

namespace jdi {

  Error::~Error() throw() {}

  const char* Error::what() const throw() {
    return(SDL_GetError());
  }
  
}  // end namespace jdi

