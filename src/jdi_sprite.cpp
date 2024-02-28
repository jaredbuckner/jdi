// File: jdi_sprite.cpp
// ----
// Implementing Sprite!

#include <SDL_image.h>

#include "jdi.hpp"

namespace jdi {

  Sprite::~Sprite() {}
  
  surface_ptr Sprite::claimSurface(SDL_Surface* sdl_surface,
                                   int elementW, int elementH) {
    _surface.reset();
    _surface = sdl_shared(sdl_surface);
    w = (elementW < _surface->w) ? _surface->w : elementW;
    h = (elementH < _surface->h) ? _surface->h : elementH;
    rows = _surface->w / w;
    cols = _surface->h / h;
    return(_surface);
  }

  bool Sprite::getBBox(SDL_Rect* bboxRect, int element) const {
    bool isInRange = (0 <= element && element < rows * cols);
    if(isInRange && bboxRect != nullptr) {
      bboxRect->x = element % cols;
      bboxRect->y = element / cols;
      bboxRect->w = w;
      bboxRect->h = h;
    }
    return(isInRange);
  }

  sprite_ptr Sprite::createFromImage(const char* file,
                                     int elementW,
                                     int elementH) {
    sprite_ptr reply = sprite_ptr(new Sprite);
    reply->claimSurface(IMG_Load(file),
                        elementW,
                        elementH);

    return(reply);
  }
  
} // end namespace jdi
