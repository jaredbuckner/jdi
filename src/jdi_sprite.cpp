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
    _w = (elementW == 0 || _surface->w < elementW) ? _surface->w : elementW;
    _h = (elementH == 0 || _surface->h < elementH) ? _surface->h : elementH;
    _cols = _surface->w / _w;
    _rows = _surface->h / _h;
    return(_surface);
  }

  surface_ptr Sprite::strokeText(font_ptr font,
                                 const char* text,
                                 Uint32 wrapLength,
                                 Color fg,
                                 Color bg) {
    _surface.reset();
    
    // What to do if text is nothing?  We'll render a single space.
    if(text == nullptr || text[0] == 0) { text = " "; }
    
    if(wrapLength == 0) {
      if(bg == Color::transparent()) {
        claimSurface(TTF_RenderUTF8_Blended(font.get(),
                                            text,
                                            fg.getSDL()));
      } else {
        claimSurface(TTF_RenderUTF8_LCD(font.get(),
                                        text,
                                        fg.getSDL(),
                                        bg.getSDL()));
      }
    } else {
      if(bg == Color::transparent()) {
        claimSurface(TTF_RenderUTF8_Blended_Wrapped(font.get(),
                                                    text,
                                                    fg.getSDL(),
                                                    wrapLength));
      } else {
        claimSurface(TTF_RenderUTF8_LCD_Wrapped(font.get(),
                                                text,
                                                fg.getSDL(),
                                                bg.getSDL(),
                                                wrapLength));        
      }
    }
    
    return(_surface);
  }

  bool Sprite::getSrcBBox(SDL_Rect* bboxRect, int element) const {
    bool isInRange = (0 <= element && element < _rows * _cols);
    if(isInRange && bboxRect != nullptr) {
      bboxRect->x = (element % _cols) * _w;
      bboxRect->y = (element / _cols) * _h;
      bboxRect->w = _w;
      bboxRect->h = _h;
    }
    return(isInRange);
  }

  bool Sprite::selectSrcBBox(SDL_Rect* bboxRect,
                             const SDL_Rect* selRect,
                             int element) const {
    bool isInRange = (0 <= element && element < _rows * _cols);
    if(isInRange) {
      SDL_Rect baseRect { 0, 0, _w, _h };
      if(bboxRect == nullptr) {
        isInRange = (SDL_HasIntersection(&baseRect, selRect) == SDL_TRUE);
      } else {
        isInRange = (SDL_IntersectRect(&baseRect, selRect, bboxRect) == SDL_TRUE);
        if(isInRange) {
          bboxRect->x += (element % _cols) * _w;
          bboxRect->y += (element / _cols) * _h;
        }
      }
    }
    return(isInRange);
  }


  

  bool Sprite::drawFull(renderer_ptr renderer,
                        const SDL_Point* tgtPoint,
                        int element) const {
    SDL_Rect srcRect;
    SDL_Rect tgtRect;
    
    if(!getSrcBBox(&srcRect, element)) { return(false); }
    tgtRect = {tgtPoint->x, tgtPoint->y,
               srcRect.w, srcRect.h };
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcRect,
                   &tgtRect);
    return(true);
  }

  bool Sprite::drawFull(renderer_ptr renderer,
                        const SDL_Rect* tgtRect,
                        int element) const {
    SDL_Rect srcRect;
    
    if(!getSrcBBox(&srcRect, element)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcRect,
                   tgtRect);
    return(true);
  }

  bool Sprite::drawSelect(renderer_ptr renderer,
                          const SDL_Rect* selRect,
                          const SDL_Point* tgtPoint,
                          int element) const {
    SDL_Rect srcRect;
    SDL_Rect tgtRect;
    
    if(!selectSrcBBox(&srcRect, selRect, element)) { return(false); }
    tgtRect = {tgtPoint->x, tgtPoint->y,
               srcRect.w, srcRect.h };
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcRect,
                   &tgtRect);
    return(true);
  }

  bool Sprite::drawSelect(renderer_ptr renderer,
                          const SDL_Rect* selRect,
                          const SDL_Rect* tgtRect,
                          int element) const {
    SDL_Rect srcRect;
    
    if(!selectSrcBBox(&srcRect, selRect, element)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcRect,
                   tgtRect);
    return(true);
  }

  bool Sprite::drawFullClipped(renderer_ptr renderer,
                               const SDL_Point* tgtPoint,
                               const SDL_Rect* clipRect,
                               int element) const {
    SDL_Rect srcRect;
    SDL_Rect srcMaskRect;
    SDL_Rect tgtRect;
    SDL_Rect tgtMaskRect;

    if(!getSrcBBox(&srcRect, element)) { return(false); }
    tgtRect = {tgtPoint->x, tgtPoint->y,
               srcRect.w, srcRect.h };
    if(!createMaskRects(&srcRect, &tgtRect, clipRect,
                        &srcMaskRect, &tgtMaskRect)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcMaskRect,
                   &tgtMaskRect);
    return(true);
  }

  bool Sprite::drawFullClipped(renderer_ptr renderer,
                               const SDL_Rect* tgtRect,
                               const SDL_Rect* clipRect,
                               int element) const {
    SDL_Rect srcRect;
    SDL_Rect srcMaskRect;
    SDL_Rect tgtMaskRect;

    if(!getSrcBBox(&srcRect, element)) { return(false); }
    if(!createMaskRects(&srcRect, tgtRect, clipRect,
                        &srcMaskRect, &tgtMaskRect)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcMaskRect,
                   &tgtMaskRect);
    return(true);
  }

  bool Sprite::drawSelectClipped(renderer_ptr renderer,
                                 const SDL_Rect* selRect,
                                 const SDL_Point* tgtPoint,
                                 const SDL_Rect* clipRect,
                                 int element) const {
    SDL_Rect srcRect;
    SDL_Rect srcMaskRect;
    SDL_Rect tgtRect;
    SDL_Rect tgtMaskRect;

    if(!selectSrcBBox(&srcRect, selRect, element)) { return(false); }
    tgtRect = {tgtPoint->x, tgtPoint->y,
               srcRect.w, srcRect.h };
    if(!createMaskRects(&srcRect, &tgtRect, clipRect,
                        &srcMaskRect, &tgtMaskRect)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcMaskRect,
                   &tgtMaskRect);
    return(true);
  }

  bool Sprite::drawSelectClipped(renderer_ptr renderer,
                                 const SDL_Rect* selRect,
                                 const SDL_Rect* tgtRect,
                                 const SDL_Rect* clipRect,
                                 int element) const {
    SDL_Rect srcRect;
    SDL_Rect srcMaskRect;
    SDL_Rect tgtMaskRect;

    if(!selectSrcBBox(&srcRect, selRect, element)) { return(false); }
    if(!createMaskRects(&srcRect, tgtRect, clipRect,
                        &srcMaskRect, &tgtMaskRect)) { return(false); }
    SDL_RenderCopy(renderer.get(),
                   _texture.get(),
                   &srcMaskRect,
                   &tgtMaskRect);
    return(true);
  }

  bool Sprite::createMaskRects(const SDL_Rect* srcRect,
                               const SDL_Rect* tgtRect,
                               const SDL_Rect* clipRect,
                               SDL_Rect* srcMaskRect,
                               SDL_Rect* tgtMaskRect) {
    if(SDL_IntersectRect(tgtRect, clipRect, tgtMaskRect)) {
      srcMaskRect->x = (tgtMaskRect->x - tgtRect->x) * srcRect->w / tgtRect->w + srcRect->x;
      srcMaskRect->y = (tgtMaskRect->y - tgtRect->y) * srcRect->h / tgtRect->h + srcRect->y;
      srcMaskRect->w = tgtMaskRect->w * srcRect->w / tgtRect->w;
      srcMaskRect->h = tgtMaskRect->h * srcRect->h / tgtRect->h;
      
      return(true);
    } else {
      return(false);
    }
  }

  sprite_ptr Sprite::createEmpty(int elementW,
                                 int elementH,
                                 int cols,
                                 int rows) {
    sprite_ptr reply = sprite_ptr(new Sprite);
    reply->claimSurface(SDL_CreateRGBSurfaceWithFormat(0,
                                                       elementW * cols,
                                                       elementH * rows,
                                                       32,
                                                       SDL_PIXELFORMAT_RGBA32),
                        elementW, elementH);
    return(reply);
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
