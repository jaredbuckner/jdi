// File: jdi_text.cpp
// ----
// Implementing text rendering

#include "jdi.hpp"

namespace jdi {

  Text::~Text() {}

  surface_ptr Text::drawText() {
    _surface.reset();

    if(_font != nullptr &&
       !_str.empty()) {
      if(_wrap > 0) {
        if(_bgColor == Color::transparent()) {
          _surface = sdl_shared(TTF_RenderUTF8_Blended_Wrapped(_font.get(),
                                                               _str.c_str(),
                                                               _fgColor.getSDL(),
                                                               _wrap));
        } else {
          _surface = sdl_shared(TTF_RenderUTF8_LCD_Wrapped(_font.get(),
                                                           _str.c_str(),
                                                           _fgColor.getSDL(),
                                                           _bgColor.getSDL(),
                                                           _wrap));
        }
      } else {
        if(_bgColor == Color::transparent()) {
          _surface = sdl_shared(TTF_RenderUTF8_Blended(_font.get(),
                                                       _str.c_str(),
                                                       _fgColor.getSDL()));
        } else {
          _surface = sdl_shared(TTF_RenderUTF8_LCD(_font.get(),
                                                   _str.c_str(),
                                                   _fgColor.getSDL(),
                                                   _bgColor.getSDL()));
        }
      }
    }
    return(_surface);
  }

  
  
} // end namespace jdi
