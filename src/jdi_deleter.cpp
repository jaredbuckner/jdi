// File: jdi_deleter.cpp
// ----
// Teach std::shared_ptr how to delete SDL items.

#include "jdi.hpp"

namespace jdi {

  void Deleter::operator()(Mix_Chunk* chunk) const {
    Mix_FreeChunk(chunk);
  }
  
  void Deleter::operator()(TTF_Font* font) const {
    TTF_CloseFont(font);
  }

  void Deleter::operator()(SDL_Joystick* joystick) const {
    SDL_JoystickClose(joystick);
  }

  void Deleter::operator()(Mix_Music* music) const {
    Mix_FreeMusic(music);
  }

  void Deleter::operator()(SDL_Renderer* renderer) const {
    SDL_DestroyRenderer(renderer);
  }
    
  void Deleter::operator()(SDL_Surface* surface) const {
    SDL_FreeSurface(surface);
  }

  void Deleter::operator()(SDL_Texture* texture) const {
    SDL_DestroyTexture(texture);
  }

  void Deleter::operator()(SDL_Window* window) const {
    SDL_DestroyWindow(window);
  }
  
} // end namespace jdi
