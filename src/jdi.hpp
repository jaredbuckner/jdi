// File: jdi.hpp
// ----
// Jared's Draw Interface.  This gives me a way to deal with SDI2 that I can
// understand and manipulate safely.

#ifndef _JDI_HPP_
#define _JDI_HPP_

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

namespace jdi {

  // JDI Class Predeclarations
  class Color;
  class Engine;
  class Grid;
  class Sprite;
  class Text;
  class Widget;
  
  // SDI Handles
  typedef std::shared_ptr<Mix_Chunk>    chunk_ptr;
  typedef std::shared_ptr<TTF_Font>     font_ptr;
  typedef std::shared_ptr<SDL_Joystick> joystick_ptr;
  typedef std::shared_ptr<Mix_Music>    music_ptr;
  typedef std::shared_ptr<SDL_Renderer> renderer_ptr;
  typedef std::shared_ptr<SDL_Surface>  surface_ptr;
  typedef std::shared_ptr<SDL_Texture>  texture_ptr;
  typedef std::shared_ptr<SDL_Window>   window_ptr;

  // JDI Handles
  typedef std::shared_ptr<Engine>       engine_ptr;
  typedef std::shared_ptr<Grid>         grid_ptr;
  typedef std::shared_ptr<Sprite>       sprite_ptr;
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
    void operator()(Mix_Chunk* chunk) const;
    void operator()(TTF_Font* font) const;
    void operator()(SDL_Joystick* joystick) const;
    void operator()(Mix_Music* music) const;
    void operator()(SDL_Renderer* renderer) const;
    void operator()(SDL_Surface* surface) const;
    void operator()(SDL_Texture* texture) const;
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
  
  // A clipped render copy.  The clip rect is in relation to tgtRect.
  // Returns true if any portion of src was rendered into tgt.
  inline bool clipped_render_copy(renderer_ptr renderer,
                                  texture_ptr texture,
                                  const SDL_Rect* srcRect,
                                  const SDL_Rect* tgtRect,
                                  const SDL_Rect* clipRect) {
    SDL_Rect maskedTgtRect;
    if(SDL_IntersectRect(tgtRect, clipRect, &maskedTgtRect)) {
      SDL_Rect maskedSrcRect;
      
      maskedSrcRect.x = (maskedTgtRect.x - tgtRect->x) * srcRect->w / tgtRect->w + srcRect->x;
      maskedSrcRect.y = (maskedTgtRect.y - tgtRect->y) * srcRect->h / tgtRect->h + srcRect->y;
      maskedSrcRect.w = maskedTgtRect.w * srcRect->w / tgtRect->w;
      maskedSrcRect.h = maskedTgtRect.h * srcRect->h / tgtRect->h;

      SDL_RenderCopy(renderer.get(),
                     texture.get(),
                     &maskedSrcRect,
                     &maskedTgtRect);
      return(true);
    } else {
      return(false);
    }
  }
                                  
  
} // end namespace jdi


#include "jdi_color.hpp"
#include "jdi_engine.hpp"
#include "jdi_sprite.hpp"
#include "jdi_widget.hpp"

#include "jdi_grid.hpp"

#endif // _JDI_HPP_
