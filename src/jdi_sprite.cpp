// File: jdi_sprite.cpp
// ----
// Implementing Sprite!

#include "jdi.hpp"

namespace jdi {

  Sprite::~Sprite() {}
  
  surface_ptr Sprite::claimSurface(SDL_Surface* sdl_surface,
                                   int elementW, int elementH) {
    _surface.reset();
    _surface = sdl_shared(sdl_surface);
    w = (elementW == 0 || _surface->w < elementW) ? _surface->w : elementW;
    h = (elementH == 0 || _surface->h < elementH) ? _surface->h : elementH;
    cols = _surface->w / w;
    rows = _surface->h / h;
    return(_surface);
  }

  bool Sprite::getBBox(SDL_Rect* bboxRect, int element) const {
    bool isInRange = (0 <= element && element < rows * cols);
    if(isInRange && bboxRect != nullptr) {
      bboxRect->x = (element % cols) * w;
      bboxRect->y = (element / cols) * h;
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
