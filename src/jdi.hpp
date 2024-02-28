// File: jdi.hpp
// ----
// Jared's Draw Interface.  This gives me a way to deal with SDI2 that I can
// understand and manipulate safely.

#include <memory>
#include <stdexcept>
#include <vector>

#include <SDL.h>

namespace jdi {

  // JDI Class Predeclarations
  class Color;
  class Engine;
  class Grid;
  class Widget;
  
  // SDI Handles
  typedef std::shared_ptr<SDL_Surface>  surface_ptr;
  typedef std::shared_ptr<SDL_Texture>  texture_ptr;
  typedef std::shared_ptr<SDL_Renderer> renderer_ptr;
  typedef std::shared_ptr<SDL_Window>   window_ptr;

  // JDI Handles
  typedef std::shared_ptr<Engine>       engine_ptr;
  typedef std::shared_ptr<Grid>         grid_ptr;
  typedef std::shared_ptr<Widget>       widget_ptr;

  // JDI Directions
  enum direction_type {
    JDI_NONE = 0,
    JDI_N    = 1,
    JDI_S    = 2,
    JDI_NS   = 3,
    JDI_E    = 4,
    JDI_NE   = 5,
    JDI_SE   = 6,
    JDI_NSE  = 7,
    JDI_W    = 8,
    JDI_NW   = 9,
    JDI_SW   = 10,
    JDI_NSW  = 11,
    JDI_EW   = 12,
    JDI_NEW  = 13,
    JDI_SEW  = 14,
    JDI_NSEW = 15,
  };
  
  ////
  // A deleter which performs free-equivalents for SDL objects
  ////
  class Deleter {
  public:
    void operator()(SDL_Surface* surface) const;
    void operator()(SDL_Texture* texture) const;
    void operator()(SDL_Renderer* renderer) const;
    void operator()(SDL_Window* window) const;
  }; // end class Deleter

  
  ////
  // An error exception for SDL errors
  ////
  class Error : public std::exception {
  private:
    const char* _what;
    const char* _where;
    
  public:
    Error(const char* where=0) throw();
    virtual ~Error() throw();
    virtual const char* what() const throw();
    const char* where() const throw();
  }; // end class Error
  

  
  
  //// INLINES ////

  // Deleter
  template <typename T>
  inline std::shared_ptr<T> sdl_shared(T* obj) {
    if(obj == 0) throw(Error());
    return(std::shared_ptr<T>(obj, Deleter()));
  }

  
  // Error
  template <typename V>
  inline V safely(V value, const char* where=0) { if(value < 0) throw(Error(where)); return(value); }
  
  
} // end namespace jdi


#include "jdi_color.hpp"
#include "jdi_engine.hpp"
#include "jdi_widget.hpp"

#include "jdi_grid.hpp"
